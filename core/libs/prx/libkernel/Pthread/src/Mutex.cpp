#include "../include/Pthread.hpp"
#include <chrono>
#include <thread>

static constexpr int SCE_OK = 0;
static constexpr int SCE_KERNEL_ERROR_EPERM = 0x80020001;
static constexpr int SCE_KERNEL_ERROR_ENOMEM = 0x8002000C;
static constexpr int SCE_KERNEL_ERROR_EBUSY = 0x80020010;
static constexpr int SCE_KERNEL_ERROR_EINVAL = 0x80020016;
static constexpr int SCE_KERNEL_ERROR_EDEADLK = 0x8002000B;
static constexpr int SCE_KERNEL_ERROR_ENOTSUP = 0x8002002D;
static constexpr int SCE_KERNEL_ERROR_ETIMEDOUT = 0x8002003C;

extern "C" {

int APS5_VABI scePthreadMutexattrInit(PthreadMutexattr* attr) {
    if (!attr) return SCE_KERNEL_ERROR_EINVAL;
    auto* p = new (std::nothrow) PthreadMutexattrPrivate{MutexType::ErrorCheck};
    if (!p) return SCE_KERNEL_ERROR_ENOMEM;
    *attr = p;
    return SCE_OK;
}

int APS5_VABI scePthreadMutexattrDestroy(PthreadMutexattr* attr) {
    if (!attr || !*attr) return SCE_KERNEL_ERROR_EINVAL;
    delete *attr;
    *attr = nullptr;
    return SCE_OK;
}

int APS5_VABI scePthreadMutexattrSettype(PthreadMutexattr* attr, int type) {
    if (!attr || !*attr) return SCE_KERNEL_ERROR_EINVAL;
    switch (type) {
    case 1: (*attr)->type = MutexType::ErrorCheck; break;
    case 2: (*attr)->type = MutexType::Recursive; break;
    case 3: (*attr)->type = MutexType::Normal; break;
    default: return SCE_KERNEL_ERROR_EINVAL;
    }
    return SCE_OK;
}

int APS5_VABI scePthreadMutexattrSetprotocol(PthreadMutexattr* attr, int protocol) {
    if (!attr || !*attr) return SCE_KERNEL_ERROR_EINVAL;
    if (protocol == 0) return SCE_OK;
    if (protocol == 1 || protocol == 2) return SCE_KERNEL_ERROR_ENOTSUP;
    return SCE_KERNEL_ERROR_EINVAL;
}

int APS5_VABI scePthreadMutexInit(PthreadMutex* mutex, const PthreadMutexattr* attr, const char*) {
    if (!mutex) return SCE_KERNEL_ERROR_EINVAL;
    MutexType t = MutexType::ErrorCheck;
    if (attr && *attr) t = (*attr)->type;
    auto* p = new (std::nothrow) PthreadMutexPrivate();
    if (!p) return SCE_KERNEL_ERROR_ENOMEM;
    p->_type = t;
    *mutex = p;
    return SCE_OK;
}

int APS5_VABI scePthreadMutexDestroy(PthreadMutex* mutex) {
    if (!mutex || !*mutex) return SCE_KERNEL_ERROR_EINVAL;
    delete *mutex;
    *mutex = nullptr;
    return SCE_OK;
}

int APS5_VABI scePthreadMutexLock(PthreadMutex* mutex) {
    if (!mutex || !*mutex) return SCE_KERNEL_ERROR_EINVAL;
    auto* m = *mutex;
    const auto tid = std::this_thread::get_id();
    if (m->_type == MutexType::Recursive) {
        m->_rmtx.lock();
        m->_owner.store(tid, std::memory_order_relaxed);
        ++m->_count;
        return SCE_OK;
    }
    if (m->_type == MutexType::ErrorCheck && m->_owner.load(std::memory_order_acquire) == tid)
        return SCE_KERNEL_ERROR_EDEADLK;
    m->_mtx.lock();
    m->_owner.store(tid, std::memory_order_relaxed);
    return SCE_OK;
}

int APS5_VABI scePthreadMutexTrylock(PthreadMutex* mutex) {
    if (!mutex || !*mutex) return SCE_KERNEL_ERROR_EINVAL;
    auto* m = *mutex;
    const auto tid = std::this_thread::get_id();
    if (m->_type == MutexType::Recursive) {
        if (!m->_rmtx.try_lock()) return SCE_KERNEL_ERROR_EBUSY;
        m->_owner.store(tid, std::memory_order_relaxed);
        ++m->_count;
        return SCE_OK;
    }
    if (m->_owner.load(std::memory_order_acquire) == tid)
        return m->_type == MutexType::ErrorCheck ? SCE_KERNEL_ERROR_EDEADLK : SCE_KERNEL_ERROR_EBUSY;
    if (!m->_mtx.try_lock()) return SCE_KERNEL_ERROR_EBUSY;
    m->_owner.store(tid, std::memory_order_relaxed);
    return SCE_OK;
}

int APS5_VABI scePthreadMutexTimedlock(PthreadMutex* mutex, KernelUseconds usec) {
    if (!mutex || !*mutex) return SCE_KERNEL_ERROR_EINVAL;
    auto* m = *mutex;
    const auto tid = std::this_thread::get_id();
    const auto wait = std::chrono::microseconds(usec);
    if (m->_type == MutexType::Recursive) {
        if (!m->_rmtx.try_lock_for(wait)) return SCE_KERNEL_ERROR_ETIMEDOUT;
        m->_owner.store(tid, std::memory_order_relaxed);
        ++m->_count;
        return SCE_OK;
    }
    if (m->_type == MutexType::ErrorCheck && m->_owner.load(std::memory_order_acquire) == tid)
        return SCE_KERNEL_ERROR_EDEADLK;
    if (!m->_mtx.try_lock_for(wait)) return SCE_KERNEL_ERROR_ETIMEDOUT;
    m->_owner.store(tid, std::memory_order_relaxed);
    return SCE_OK;
}

int APS5_VABI scePthreadMutexUnlock(PthreadMutex* mutex) {
    if (!mutex || !*mutex) return SCE_KERNEL_ERROR_EINVAL;
    auto* m = *mutex;
    if (m->_type == MutexType::ErrorCheck || m->_type == MutexType::Normal) {
        if (m->_owner.load(std::memory_order_acquire) != std::this_thread::get_id())
            return SCE_KERNEL_ERROR_EPERM;
    }
    if (m->_type == MutexType::Recursive) {
        if (m->_count == 0 || m->_owner.load(std::memory_order_acquire) != std::this_thread::get_id())
            return SCE_KERNEL_ERROR_EPERM;
        if (--m->_count == 0) m->_owner.store(std::thread::id{}, std::memory_order_relaxed);
        m->_rmtx.unlock();
        return SCE_OK;
    }
    m->_owner.store(std::thread::id{}, std::memory_order_relaxed);
    m->_mtx.unlock();
    return SCE_OK;
}

}
