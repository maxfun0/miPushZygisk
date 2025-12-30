#include "hook.h"
#include "logging.h"
#include <string>
#include <cstring>

using namespace std;

jstring (*orig_native_get)(JNIEnv *env, jclass clazz, jstring keyJ, jstring defJ);

jstring my_native_get(JNIEnv *env, jclass clazz, jstring keyJ, jstring defJ) {
    const char *key = env->GetStringUTFChars(keyJ, nullptr);
    const char *def = env->GetStringUTFChars(defJ, nullptr);
    jstring hooked_result = nullptr;

    // ===== 1. 核心品牌伪装（必须）=====
    if (strcmp(key, "ro.product.brand") == 0) {
        hooked_result = env->NewStringUTF("Xiaomi");
    } else if (strcmp(key, "ro.product.manufacturer") == 0) {
        hooked_result = env->NewStringUTF("Xiaomi");
    
    // ===== 2. 屏蔽其他品牌特征 =====
    } else if (strcmp(key, "ro.build.version.emui") == 0 ||
               strcmp(key, "ro.build.hw_emui_api_level") == 0 ||
               strcmp(key, "ro.vendor.build.emui") == 0 ||
               strcmp(key, "ro.huawei.build.display.id") == 0) {
        // 屏蔽华为特征 - 返回空值
        hooked_result = env->NewStringUTF("");
    
    } else if (strcmp(key, "ro.vivo.os.name") == 0 ||
               strcmp(key, "ro.vivo.os.version") == 0 ||
               strcmp(key, "ro.vivo.build.version") == 0 ||
               strcmp(key, "ro.funtouch.version") == 0) {
        // 屏蔽vivo特征
        hooked_result = env->NewStringUTF("");
    
    } else if (strcmp(key, "ro.oppo.version") == 0 ||
               strcmp(key, "ro.oppo.version_name") == 0 ||
               strcmp(key, "ro.coloros.version") == 0 ||
               strcmp(key, "ro.build.version.opporom") == 0) {
        // 屏蔽OPPO特征
        hooked_result = env->NewStringUTF("");
    
    } else if (strcmp(key, "ro.samsung.smd.version") == 0 ||
               strcmp(key, "ro.build.scafe.version") == 0 ||
               strcmp(key, "ro.build.PDA") == 0 ||
               strcmp(key, "ro.build.changelist") == 0) {
        // 屏蔽三星特征
        hooked_result = env->NewStringUTF("");
    
    } else if (strcmp(key, "ro.oneplus.version") == 0 ||
               strcmp(key, "ro.oxygen.version") == 0 ||
               strcmp(key, "ro.rom.version") == 0) {
        // 屏蔽一加特征
        hooked_result = env->NewStringUTF("");
    
    } else if (strcmp(key, "ro.meizu.product.model") == 0 ||
               strcmp(key, "ro.meizu.build.display.id") == 0 ||
               strcmp(key, "ro.build.meizu.rom") == 0) {
        // 屏蔽魅族特征
        hooked_result = env->NewStringUTF("");
    
    // ===== 3. 设备型号伪装 =====
    } else if (strcmp(key, "ro.product.model") == 0) {
        // 使用更通用的型号名称
        hooked_result = env->NewStringUTF("Xiaomi 13");
    
    // ===== 4. 设备识别信息 =====
    } else if (strcmp(key, "ro.product.device") == 0) {
        hooked_result = env->NewStringUTF("fuxi");
    } else if (strcmp(key, "ro.product.name") == 0) {
        hooked_result = env->NewStringUTF("fuxi");
    
    // ===== 5. MIUI/HyperOS信息 =====
    } else if (strcmp(key, "ro.miui.ui.version.code") == 0) {
        hooked_result = env->NewStringUTF("14");
    } else if (strcmp(key, "ro.miui.ui.version.name") == 0) {
        hooked_result = env->NewStringUTF("V14");
    } else if (strcmp(key, "ro.miui.version.code_time") == 0) {
        hooked_result = env->NewStringUTF("231221");
    
    // ===== 6. 屏蔽非MIUI系统特征 =====
    } else if (strcmp(key, "ro.oplus.version") == 0 ||
               strcmp(key, "ro.oplus.os.version") == 0 ||
               strcmp(key, "ro.realme.version") == 0 ||
               strcmp(key, "ro.smartisan.version") == 0) {
        // 屏蔽其他ROM特征
        hooked_result = env->NewStringUTF("");
    
    // ===== 7. Android版本信息 =====
    } else if (strcmp(key, "ro.build.version.release") == 0) {
        hooked_result = env->NewStringUTF("14");
    } else if (strcmp(key, "ro.build.version.sdk") == 0) {
        hooked_result = env->NewStringUTF("34");
    } else if (strcmp(key, "ro.build.version.incremental") == 0) {
        hooked_result = env->NewStringUTF("V14.0.6.0.UMCCNXM");
    
    // ===== 8. 地区信息 =====
    } else if (strcmp(key, "ro.product.locale.region") == 0) {
        hooked_result = env->NewStringUTF("CN");
    } else if (strcmp(key, "ro.product.mod_device") == 0) {
        hooked_result = env->NewStringUTF("fuxi_cn");
    
    // ===== 9. 屏蔽硬件特征 =====
    } else if (strcmp(key, "ro.hardware") == 0) {
        hooked_result = env->NewStringUTF("qcom");
    } else if (strcmp(key, "ro.product.board") == 0) {
        hooked_result = env->NewStringUTF("kalama");
    
    // ===== 10. 系统特性 - 避免过度修改 =====
    } else if (strcmp(key, "ro.build.characteristics") == 0) {
        // 谨慎修改，可能影响应用行为
        if (def && strstr(def, "watch")) {
            // 如果原值包含watch，改为phone
            hooked_result = env->NewStringUTF("default,phone");
        }
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
    LOGD("hook Build - Xiaomi 13 (屏蔽其他品牌)\n");
    
    jclass build_class = env->FindClass("android/os/Build");
    jclass build_version_class = env->FindClass("android/os/Build$VERSION");
    
    if (build_class != nullptr) {
        // ===== 设置核心Build字段 =====
        setBuildField(env, build_class, "BRAND", "Xiaomi");
        setBuildField(env, build_class, "MANUFACTURER", "Xiaomi");
        setBuildField(env, build_class, "MODEL", "Xiaomi 13"); // 使用友好名称
        
        // 设备标识
        setBuildField(env, build_class, "DEVICE", "fuxi");
        setBuildField(env, build_class, "PRODUCT", "fuxi");
        
        // 移除指纹修改 - 避免崩溃
        // setBuildField(env, build_class, "FINGERPRINT", "...");
        
        env->DeleteLocalRef(build_class);
    }
    
    if (build_version_class != nullptr) {
        // 设置VERSION字段
        setBuildField(env, build_version_class, "RELEASE", "14");
        setBuildField(env, build_version_class, "INCREMENTAL", "V14.0.6.0.UMCCNXM");
        setBuildIntField(env, build_version_class, "SDK_INT", 34);
        
        env->DeleteLocalRef(build_version_class);
    }
    
    LOGD("hook Build done (已屏蔽其他品牌特征)");
}

void hookSystemProperties(JNIEnv *env, zygisk::Api *api) {
    LOGD("hook SystemProperties - 屏蔽其他品牌\n");

    JNINativeMethod targetHookMethods[] = {
        {"native_get", "(Ljava/lang/String;Ljava/lang/String;)Ljava/lang/String;", (void *) my_native_get},
    };

    api->hookJniNativeMethods(env, "android/os/SystemProperties", targetHookMethods, 1);
    *(void **) &orig_native_get = targetHookMethods[0].fnPtr;

    LOGD("hook SystemProperties done: %p\n", orig_native_get);
}

void Hook::hook() {
    LOGD("开始hook - 屏蔽其他品牌特征");
    hookBuild(env);
    hookSystemProperties(env, api);
    LOGD("hook完成");
}
