#include "hook.h"
#include "logging.h"
#include <string>

using namespace std;

jstring (*orig_native_get)(JNIEnv *env, jclass clazz, jstring keyJ, jstring defJ);

jstring my_native_get(JNIEnv *env, jclass clazz, jstring keyJ, jstring defJ) {
    const char *key = env->GetStringUTFChars(keyJ, nullptr);
    const char *def = env->GetStringUTFChars(defJ, nullptr);
    jstring hooked_result = nullptr;

    // 基础品牌信息
    if (strcmp(key, "ro.product.brand") == 0) {
        hooked_result = env->NewStringUTF("Xiaomi");
    } else if (strcmp(key, "ro.product.manufacturer") == 0) {
        hooked_result = env->NewStringUTF("Xiaomi");
    } else if (strcmp(key, "ro.product.model") == 0) {
        hooked_result = env->NewStringUTF("Xiaomi 17");
    
    // 设备识别信息
    } else if (strcmp(key, "ro.product.device") == 0) {
        hooked_result = env->NewStringUTF("aurora");
    } else if (strcmp(key, "ro.product.name") == 0) {
        hooked_result = env->NewStringUTF("aurora");
    } else if (strcmp(key, "ro.hardware") == 0) {
        hooked_result = env->NewStringUTF("qcom");
    
    // MIUI/HyperOS信息
    } else if (strcmp(key, "ro.miui.ui.version.code") == 0) {
        hooked_result = env->NewStringUTF("21");
    } else if (strcmp(key, "ro.miui.ui.version.name") == 0) {
        hooked_result = env->NewStringUTF("HyperOS 2.0");
    } else if (strcmp(key, "ro.miui.version.code_time") == 0) {
        hooked_result = env->NewStringUTF("2510010000");
    } else if (strcmp(key, "ro.miui.build.region") == 0) {
        hooked_result = env->NewStringUTF("CN");
    } else if (strcmp(key, "ro.miui.internal.storage") == 0) {
        hooked_result = env->NewStringUTF("/data/media/0");
    
    // Android版本信息
    } else if (strcmp(key, "ro.build.version.release") == 0) {
        hooked_result = env->NewStringUTF("15");
    } else if (strcmp(key, "ro.build.version.sdk") == 0) {
        hooked_result = env->NewStringUTF("35");
    } else if (strcmp(key, "ro.build.version.incremental") == 0) {
        hooked_result = env->NewStringUTF("OS2.0.1.0.ULACNXM");
    } else if (strcmp(key, "ro.build.version.security_patch") == 0) {
        hooked_result = env->NewStringUTF("2025-10-01");
    } else if (strcmp(key, "ro.build.version.preview_sdk") == 0) {
        hooked_result = env->NewStringUTF("1");
    
    // 构建信息
    } else if (strcmp(key, "ro.build.fingerprint") == 0) {
        hooked_result = env->NewStringUTF("Xiaomi/aurora/aurora:15/U1B1.250928.001/OS2.0.1.0.ULACNXM:user/release-keys");
    } else if (strcmp(key, "ro.build.display.id") == 0) {
        hooked_result = env->NewStringUTF("OS2.0.1.0.ULACNXM");
    } else if (strcmp(key, "ro.build.type") == 0) {
        hooked_result = env->NewStringUTF("user");
    } else if (strcmp(key, "ro.build.tags") == 0) {
        hooked_result = env->NewStringUTF("release-keys");
    
    // 地区信息
    } else if (strcmp(key, "ro.product.locale.region") == 0) {
        hooked_result = env->NewStringUTF("CN");
    } else if (strcmp(key, "ro.product.mod_device") == 0) {
        hooked_result = env->NewStringUTF("aurora_cn");
    } else if (strcmp(key, "ro.product.locale") == 0) {
        hooked_result = env->NewStringUTF("zh-CN");
    } else if (strcmp(key, "ro.product.locale.language") == 0) {
        hooked_result = env->NewStringUTF("zh");
    
    // 其他重要参数
    } else if (strcmp(key, "ro.miui.cust_variant") == 0) {
        hooked_result = env->NewStringUTF("cn");
    } else if (strcmp(key, "ro.debuggable") == 0) {
        hooked_result = env->NewStringUTF("0");
    } else if (strcmp(key, "persist.sys.timezone") == 0) {
        hooked_result = env->NewStringUTF("Asia/Shanghai");
    }

    env->ReleaseStringUTFChars(keyJ, key);
    env->ReleaseStringUTFChars(defJ, def);

    return hooked_result != nullptr ? hooked_result : orig_native_get(env, clazz, keyJ, defJ);
}

// 辅助函数：安全设置Build字段
void setBuildField(JNIEnv *env, jclass clazz, const char *fieldName, const char *value) {
    jfieldID fieldId = env->GetStaticFieldID(clazz, fieldName, "Ljava/lang/String;");
    if (fieldId != nullptr) {
        jstring newValue = env->NewStringUTF(value);
        env->SetStaticObjectField(clazz, fieldId, newValue);
        env->DeleteLocalRef(newValue);
    }
}

// 辅助函数：安全设置Int字段
void setBuildIntField(JNIEnv *env, jclass clazz, const char *fieldName, int value) {
    jfieldID fieldId = env->GetStaticFieldID(clazz, fieldName, "I");
    if (fieldId != nullptr) {
        env->SetStaticIntField(clazz, fieldId, value);
    }
}

void hookBuild(JNIEnv *env) {
    LOGD("hook Build for Xiaomi 17\n");
    
    jclass build_class = env->FindClass("android/os/Build");
    jclass build_version_class = env->FindClass("android/os/Build$VERSION");
    
    if (build_class != nullptr) {
        // 设置Build字段
        setBuildField(env, build_class, "BRAND", "Xiaomi");
        setBuildField(env, build_class, "MANUFACTURER", "Xiaomi");
        setBuildField(env, build_class, "MODEL", "Xiaomi 17");
        setBuildField(env, build_class, "DEVICE", "aurora");
        setBuildField(env, build_class, "PRODUCT", "aurora");
        setBuildField(env, build_class, "HARDWARE", "qcom");
        setBuildField(env, build_class, "BOARD", "aurora");
        setBuildField(env, build_class, "FINGERPRINT", 
            "Xiaomi/aurora/aurora:15/U1B1.250928.001/OS2.0.1.0.ULACNXM:user/release-keys");
        
        // 可选字段
        setBuildField(env, build_class, "SERIAL", "98KRNW4321A");
        setBuildField(env, build_class, "ID", "U1B1.250928.001");
        
        env->DeleteLocalRef(build_class);
    }
    
    if (build_version_class != nullptr) {
        // 设置VERSION字段
        setBuildField(env, build_version_class, "RELEASE", "15");
        setBuildField(env, build_version_class, "INCREMENTAL", "OS2.0.1.0.ULACNXM");
        setBuildField(env, build_version_class, "SECURITY_PATCH", "2025-10-01");
        setBuildField(env, build_version_class, "BASE_OS", 
            "Xiaomi/aurora/aurora:15/U1B1.250928.001:user/release-keys");
        
        setBuildIntField(env, build_version_class, "SDK_INT", 35);
        setBuildIntField(env, build_version_class, "PREVIEW_SDK_INT", 1);
        
        env->DeleteLocalRef(build_version_class);
    }
    
    LOGD("hook Build for Xiaomi 17 done");
}

void hookSystemProperties(JNIEnv *env, zygisk::Api *api) {
    LOGD("hook SystemProperties for Xiaomi 17\n");

    JNINativeMethod targetHookMethods[] = {
        {"native_get", "(Ljava/lang/String;Ljava/lang/String;)Ljava/lang/String;", (void *) my_native_get},
    };

    api->hookJniNativeMethods(env, "android/os/SystemProperties", targetHookMethods, 1);
    *(void **) &orig_native_get = targetHookMethods[0].fnPtr;

    LOGD("hook SystemProperties done: %p\n", orig_native_get);
}

void Hook::hook() {
    hookBuild(env);
    hookSystemProperties(env, api);
}
