#include "prx/libc/include/GuestLocale.hpp"
#include <array>
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <thread>
#include <vector>

extern "C" {
extern GuestLocale::Implementation* _ZSt21_sceLibcClassicLocale_nid_postfix;
GuestLocale::Implementation* APS5_VABI _ZNSt6locale16_GetgloballocaleEv_nid_postfix();
void APS5_VABI _ZNSt6locale5_InitEv_nid_postfix();
void APS5_VABI _ZNSt6locale5facet9_RegisterEv_nid_postfix(GuestLocale::Facet* self);
void APS5_VABI _ZNSt8_LocinfoC1EPKc_nid_postfix(GuestLocale::LocinfoStorage* self, const char* localeName);
void APS5_VABI _ZNSt8_LocinfoD1Ev_nid_postfix(GuestLocale::LocinfoStorage* self);
void APS5_VABI _ZNSt8ios_baseD2Ev_nid_postfix(GuestLocale::IosBase* self);
const short* APS5_VABI _Getpctype_nid_postfix();
const short* APS5_VABI _Getptolower_nid_postfix();
const short* APS5_VABI _Getptoupper_nid_postfix();
void APS5_VABI _Locksyslock_nid_postfix();
void APS5_VABI _Unlocksyslock_nid_postfix();

}

static void Require(bool condition) {
    if (!condition) throw std::runtime_error("Guest locale check failed");
}

template<typename TAction>
static void RequireException(TAction action) {
    bool caught = false;
    try {
        action();
    } catch (const std::exception&) {
        caught = true;
    }
    Require(caught);
}

static void CheckGuestCalls() {
    auto* locale = _ZSt21_sceLibcClassicLocale_nid_postfix;
    Require(locale != nullptr && locale == _ZNSt6locale16_GetgloballocaleEv_nid_postfix());
    _ZNSt6locale5_InitEv_nid_postfix();
    _ZNSt6locale5_InitEv_nid_postfix();
    const std::byte* table = nullptr;
    std::memcpy(&table, locale, sizeof(table));
    void (APS5_VABI *retain)(GuestLocale::Facet*) = nullptr;
    GuestLocale::Facet* (APS5_VABI *release)(GuestLocale::Facet*) = nullptr;
    std::memcpy(&retain, table + 0x10, sizeof(retain));
    std::memcpy(&release, table + 0x18, sizeof(release));
    const auto initial = locale->base.references;
    retain(&locale->base);
    Require(locale->base.references == initial + 1);
    Require(release(&locale->base) == nullptr);
    Require(locale->base.references == initial);
    RequireException([&] { release(&locale->base); });
    RequireException([&] { retain(nullptr); });
    RequireException([&] { locale->base.vtable->deleteObject(&locale->base); });
    Require(locale->base.references == initial);
    std::vector<std::thread> threads;
    for (int thread = 0; thread < 4; ++thread) {
        threads.emplace_back([&] {
            for (int iteration = 0; iteration < 10000; ++iteration) {
                retain(&locale->base);
                release(&locale->base);
            }
        });
    }
    for (auto& thread : threads) thread.join();
    Require(locale->base.references == initial);
    Require(locale->facetCount == 1 && locale->facets[0] == nullptr);
    Require(std::strcmp(locale->name, "C") == 0 && !locale->transparent);
}

static void CheckLocinfoAlignment() {
    alignas(16) std::array<std::byte, 80> bytes;
    bytes.fill(std::byte{0x5a});
    auto* storage = reinterpret_cast<GuestLocale::LocinfoStorage*>(bytes.data() + 8);
    _ZNSt8_LocinfoC1EPKc_nid_postfix(storage, "C");
    for (std::size_t index = 0; index < bytes.size(); ++index) Require(bytes[index] == (index >= 8 && index < 72 ? std::byte{} : std::byte{0x5a}));
    _ZNSt8_LocinfoD1Ev_nid_postfix(storage);
    RequireException([&] { _ZNSt8_LocinfoC1EPKc_nid_postfix(storage, "unknown-locale"); });
    RequireException([] { _ZNSt8_LocinfoC1EPKc_nid_postfix(nullptr, "C"); });
}

static void CheckCharacterTables() {
    const auto* classification = _Getpctype_nid_postfix();
    const auto* lower = _Getptolower_nid_postfix();
    const auto* upper = _Getptoupper_nid_postfix();
    Require(classification[-1] == 0 && lower[-1] == -1 && upper[-1] == -1);
    Require(classification['A'] == 0x181 && classification['a'] == 0x182);
    Require(classification['G'] == 0x101 && classification['0'] == 0x84);
    Require(classification[' '] == 0x48 && classification['\t'] == 0x68);
    Require(classification['\n'] == 0x28 && classification['!'] == 0x10);
    for (int value = 0; value < 256; ++value) {
        Require(lower[value] == (value >= 'A' && value <= 'Z' ? value + 32 : value));
        Require(upper[value] == (value >= 'a' && value <= 'z' ? value - 32 : value));
        if (value >= 128) Require(classification[value] == 0);
    }
}

static void CheckStreamDestruction() {
    struct GuardedStream {
        std::uint64_t before = 0x12345678;
        GuestLocale::IosBase stream{};
        std::uint64_t after = 0x87654321;
    } object;
    object.stream.locale = &_ZSt21_sceLibcClassicLocale_nid_postfix;
    _ZNSt8ios_baseD2Ev_nid_postfix(&object.stream);
    Require(object.stream.locale == nullptr && object.before == 0x12345678 && object.after == 0x87654321);
    object.stream.locale = &_ZSt21_sceLibcClassicLocale_nid_postfix;
    object.stream.callbacks = &object;
    RequireException([&] { _ZNSt8ios_baseD2Ev_nid_postfix(&object.stream); });
    Require(object.stream.locale == &_ZSt21_sceLibcClassicLocale_nid_postfix);
}

int main() {
    CheckGuestCalls();
    CheckLocinfoAlignment();
    CheckCharacterTables();
    CheckStreamDestruction();
    _Locksyslock_nid_postfix();
    _Locksyslock_nid_postfix();
    _Unlocksyslock_nid_postfix();
    _Unlocksyslock_nid_postfix();
    std::cout << "Guest locale checks passed\n";
}
