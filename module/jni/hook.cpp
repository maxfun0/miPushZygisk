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
        hooked_result = env->NewStringUTF("2210132C"); // 小米13系列型号，避免使用"Xiaomi 17"
    
    // 设备识别信息 - 使用已存在的设备代号
    } else if (strcmp(key, "ro.product.device") == 0) {
        hooked_result = env->NewStringUTF("fuxi");
    } else if (strcmp(key, "ro.product.name") == 0) {
        hooked_result = env->NewStringUTF("fuxi");
    } else if (strcmp(key, "ro.hardware") == 0) {
        hooked_result = env->NewStringUTF("qcom");
    } else if (strcmp(key, "ro.product.board") == 0) {
        hooked_result = env->NewStringUTF("fuxi");
    
    // 设备类型 - 明确指定为手机（关键修复）
    } else if (strcmp(key, "ro.build.characteristics") == 0) {
        hooked_result = env->NewStringUTF("default,phone");
    } else if (strcmp(key, "ro.build.flavor") == 0) {
        hooked_result = env->NewStringUTF("fuxi-user");
    
    // MIUI/HyperOS信息 - 使用真实存在的版本
    } else if (strcmp(key, "ro.miui.ui.version.code") == 0) {
        hooked_result = env->NewStringUTF("14");
    } else if (strcmp(key, "ro.miui.ui.version.name") == 0) {
        hooked_result = env->NewStringUTF("V14");
    } else if (strcmp(key, "ro.miui.version.code_time") == 0) {
        hooked_result = env->NewStringUTF("231221");
    } else if (strcmp(key, "ro.miui.build.region") == 0) {
        hooked_result = env->NewStringUTF("CN");
    } else if (strcmp(key, "ro.miui.internal.storage") == 0) {
        hooked_result = env->NewStringUTF("/data/media/0");
    
    // Android版本信息 - 使用更合理的版本
    } else if (strcmp(key, "ro.build.version.release") == 0) {
        hooked_result = env->NewStringUTF("14");
    } else if (strcmp(key, "ro.build.version.sdk") == 0) {
        hooked_result = env->NewStringUTF("34");
    } else if (strcmp(key, "ro.build.version.incremental") == 0) {
        hooked_result = env->NewStringUTF("V14.0.6.0.UMCCNXM");
    } else if (strcmp(key, "ro.build.version.security_patch") == 0) {
        hooked_result = env->NewStringUTF("2024-01-01");
    }
    
    // 构建指纹 - 使用真实存在的指纹格式
    else if (strcmp(key, "ro.build.fingerprint") == 0) {
        hooked_result = env->NewStringUTF("Xiaomi/fuxi/fuxi:14/UKQ1.231117.002/V14.0.6.0.UMCCNXM:user/release-keys");
    } else if (strcmp(key, "ro.build.display.id") == 0) {
        hooked_result = env->NewStringUTF("V14.0.6.0.UMCCNXM");
    } else if (strcmp(key, "ro.build.type") == 0) {
        hooked_result = env->NewStringUTF("user");
    } else if (strcmp(key, "ro.build.tags") == 0) {
        hooked_result = env->NewStringUTF("release-keys");
    } else if (strcmp(key, "ro.build.description") == 0) {
        hooked_result = env->NewStringUTF("fuxi-user 14 UKQ1.231117.002 V14.0.6.0.UMCCNXM release-keys");
    
    // 地区信息
    } else if (strcmp(key, "ro.product.locale.region") == 0) {
        hooked_result = env->NewStringUTF("CN");
    } else if (strcmp(key, "ro.product.mod_device") == 0) {
        hooked_result = env->NewStringUTF("fuxi_cn");
    } else if (strcmp(key, "ro.product.locale") == 0) {
        hooked_result = env->NewStringUTF("zh-CN");
    
    // 系统特性 - 关键修复，表明是手机设备
    } else if (strcmp(key, "ro.build.product") == 0) {
        hooked_result = env->NewStringUTF("fuxi");
    } else if (strcmp(key, "ro.product.system.manufacturer") == 0) {
        hooked_result = env->NewStringUTF("Xiaomi");
    } else if (strcmp(key, "ro.product.system.model") == 0) {
        hooked_result = env->NewStringUTF("2210132C");
    
    // 避免被识别为手表的关键参数
    } else if (strcmp(key, "ro.build.characteristics") == 0) {
        hooked_result = env->NewStringUTF("default,phone");
    } else if (strcmp(key, "ro.build.flavor") == 0) {
        hooked_result = env->NewStringUTF("fuxi-user");
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
    if (clazz == nullptr || fieldName == nullptr || value == nullptr) return;
    
    jfieldID fieldId = env->GetStaticFieldID(clazz, fieldName, "Ljava/lang/String;");
    if (fieldId != nullptr) {
        jstring newValue = env->NewStringUTF(value);
        env->SetStaticObjectField(clazz, fieldId, newValue);
        env->DeleteLocalRef(newValue);
    }
}

// 辅助函数：安全设置Int字段
void setBuildIntField(JNIEnv *env, jclass clazz, const char *fieldName, int value) {
    if (clazz == nullptr || fieldName == nullptr) return;
    
    jfieldID fieldId = env->GetStaticFieldID(clazz, fieldName, "I");
    if (fieldId != nullptr) {
        env->SetStaticIntField(clazz, fieldId, value);
    }
}

void hookBuild(JNIEnv *env) {
    LOGD("hook Build - Xiaomi 13\n");
    
    jclass build_class = env->FindClass("android/os/Build");
    jclass build_version_class = env->FindClass("android/os/Build$VERSION");
    
    if (build_class != nullptr) {
        // 设置Build字段 - 使用真实的小米13参数
        setBuildField(env, build_class, "BRAND", "Xiaomi");
        setBuildField(env, build_class, "MANUFACTURER", "Xiaomi");
        setBuildField(env, build_class, "MODEL", "2210132C");
        setBuildField(env, build_class, "DEVICE", "fuxi");
        setBuildField(env, build_class, "PRODUCT", "fuxi");
        setBuildField(env, build_class, "HARDWARE", "qcom");
        setBuildField(env, build_class, "BOARD", "fuxi");
        setBuildField(env, build_class, "FINGERPRINT", 
            "Xiaomi/fuxi/fuxi:14/UKQ1.231117.002/V14.0.6.0.UMCCNXM:user/release-keys");
        
        // 设备类型相关字段
        setBuildField(env, build_class, "SERIAL", "98KRNW4321A");
        setBuildField(env, build_class, "ID", "UKQ1.231117.002");
        
        env->DeleteLocalRef(build_class);
    }
    
    if (build_version_class != nullptr) {
        // 设置VERSION字段
        setBuildField(env, build_version_class, "RELEASE", "14");
        setBuildField(env, build_version_class, "INCREMENTAL", "V14.0.6.0.UMCCNXM");
        setBuildField(env, build_version_class, "SECURITY_PATCH", "2024-01-01");
        setBuildField(env, build_version_class, "BASE_OS", "");
        setBuildField(env, build_version_class, "CODENAME", "REL");
        
        setBuildIntField(env, build_version_class, "SDK_INT", 34);
        
        // 删除预览SDK相关设置，避免异常
        jfieldID preview_sdk_id = env->GetStaticFieldID(build_version_class, "PREVIEW_SDK_INT", "I");
        if (preview_sdk_id != nullptr) {
            env->SetStaticIntField(build_version_class, preview_sdk_id, 0);
        }
        
        env->DeleteLocalRef(build_version_class);
    }
    
    LOGD("hook Build done");
}

void hookSystemProperties(JNIEnv *env, zygisk::Api *api) {
    LOGD("hook SystemProperties for Xiaomi device\n");

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
