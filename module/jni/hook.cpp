#include "hook.h"
#include "logging.h"
#include <string>

using namespace std;

jstring (*orig_native_get)(JNIEnv *env, jclass clazz, jstring keyJ, jstring defJ);

jstring my_native_get(JNIEnv *env, jclass clazz, jstring keyJ, jstring defJ) {
    const char *key = env->GetStringUTFChars(keyJ, nullptr);
    const char *def = env->GetStringUTFChars(defJ, nullptr);
    jstring hooked_result = nullptr;

    // ==== 1. 品牌和制造商伪装（最高优先级）====
    if (strcmp(key, "ro.product.brand") == 0 ||
        strcmp(key, "ro.product.system.brand") == 0 ||
        strcmp(key, "ro.product.vendor.brand") == 0 ||
        strcmp(key, "ro.product.odm.brand") == 0) {
        hooked_result = env->NewStringUTF("Xiaomi");
    } else if (strcmp(key, "ro.product.manufacturer") == 0 ||
               strcmp(key, "ro.product.system.manufacturer") == 0 ||
               strcmp(key, "ro.product.vendor.manufacturer") == 0 ||
               strcmp(key, "ro.product.odm.manufacturer") == 0 ||
               strcmp(key, "ro.product.cuptsm.manufacturer") == 0 ||
               strcmp(key, "ro.serialno") == 0) {
        hooked_result = env->NewStringUTF("Xiaomi");
    
    // ==== 2. 屏蔽其他品牌特征 ====
    } else if (strcmp(key, "ro.huaqin.product.model") == 0 ||
               strcmp(key, "ro.huaqin.version.release") == 0 ||
               strcmp(key, "ro.huawei.build.display.id") == 0 ||
               strcmp(key, "ro.build.version.emui") == 0 ||
               strcmp(key, "ro.build.version.huawei") == 0 ||
               strcmp(key, "ro.vendor.build.emui") == 0 ||
               strcmp(key, "ro.build.hw_emui_api_level") == 0) {
        hooked_result = env->NewStringUTF(""); // 清空华为特征
    
    } else if (strcmp(key, "ro.vivo.os.name") == 0 ||
               strcmp(key, "ro.vivo.os.version") == 0 ||
               strcmp(key, "ro.vivo.build.version") == 0) {
        hooked_result = env->NewStringUTF(""); // 清空vivo特征
    
    } else if (strcmp(key, "ro.oppo.version") == 0 ||
               strcmp(key, "ro.oppo.version_name") == 0 ||
               strcmp(key, "ro.oppo.build.version") == 0) {
        hooked_result = env->NewStringUTF(""); // 清空OPPO特征
    
    } else if (strcmp(key, "ro.samsung.smd.version") == 0 ||
               strcmp(key, "ro.build.scafe.version") == 0 ||
               strcmp(key, "ro.build.PDA") == 0) {
        hooked_result = env->NewStringUTF(""); // 清空三星特征
    
    } else if (strcmp(key, "ro.oneplus.version") == 0 ||
               strcmp(key, "ro.oxygen.version") == 0 ||
               strcmp(key, "ro.rom.version") == 0) {
        hooked_result = env->NewStringUTF(""); // 清空一加特征
    
    } else if (strcmp(key, "ro.meizu.product.model") == 0 ||
               strcmp(key, "ro.meizu.build.display.id") == 0) {
        hooked_result = env->NewStringUTF(""); // 清空魅族特征
    
    // ==== 3. 设备型号伪装 ====
    } else if (strcmp(key, "ro.product.model") == 0 ||
               strcmp(key, "ro.product.system.model") == 0 ||
               strcmp(key, "ro.product.vendor.model") == 0 ||
               strcmp(key, "ro.product.odm.model") == 0 ||
               strcmp(key, "ro.boot.mode") == 0 ||
               strcmp(key, "ro.build.product") == 0) {
        hooked_result = env->NewStringUTF("Xiaomi 13");
    
    // ==== 4. 设备识别信息 ====
    } else if (strcmp(key, "ro.product.device") == 0 ||
               strcmp(key, "ro.product.vendor.device") == 0 ||
               strcmp(key, "ro.product.system.device") == 0) {
        hooked_result = env->NewStringUTF("fuxi");
    } else if (strcmp(key, "ro.product.name") == 0 ||
               strcmp(key, "ro.product.vendor.name") == 0) {
        hooked_result = env->NewStringUTF("fuxi");
    
    // ==== 5. MIUI/HyperOS信息 ====
    } else if (strcmp(key, "ro.miui.ui.version.code") == 0 ||
               strcmp(key, "ro.miui.ui.version.name") == 0 ||
               strcmp(key, "ro.miui.version.code_time") == 0 ||
               strcmp(key, "ro.miui.build.region") == 0 ||
               strcmp(key, "ro.miui.product.name") == 0 ||
               strcmp(key, "ro.miui.internal.storage") == 0) {
        if (strcmp(key, "ro.miui.ui.version.code") == 0) {
            hooked_result = env->NewStringUTF("14");
        } else if (strcmp(key, "ro.miui.ui.version.name") == 0) {
            hooked_result = env->NewStringUTF("V14");
        } else if (strcmp(key, "ro.miui.version.code_time") == 0) {
            hooked_result = env->NewStringUTF("231221");
        } else if (strcmp(key, "ro.miui.build.region") == 0) {
            hooked_result = env->NewStringUTF("CN");
        } else {
            hooked_result = env->NewStringUTF("fuxi");
        }
    
    // ==== 6. 屏蔽非MIUI系统特征 ====
    } else if (strcmp(key, "ro.oplus.version") == 0 ||
               strcmp(key, "ro.oplus.os.version") == 0 ||
               strcmp(key, "ro.coloros.version") == 0 ||
               strcmp(key, "ro.build.version.opporom") == 0 ||
               strcmp(key, "ro.vivo.os.build.display.id") == 0 ||
               strcmp(key, "ro.funtouch.version") == 0 ||
               strcmp(key, "ro.smartisan.version") == 0 ||
               strcmp(key, "ro.smartisan.build") == 0) {
        hooked_result = env->NewStringUTF(""); // 清空其他ROM特征
    
    // ==== 7. 构建指纹和版本 ====
    } else if (strcmp(key, "ro.build.fingerprint") == 0 ||
               strcmp(key, "ro.vendor.build.fingerprint") == 0 ||
               strcmp(key, "ro.bootimage.build.fingerprint") == 0) {
        hooked_result = env->NewStringUTF("Xiaomi/fuxi/fuxi:14/UKQ1.231117.002/V14.0.6.0.UMCCNXM:user/release-keys");
    } else if (strcmp(key, "ro.build.version.incremental") == 0 ||
               strcmp(key, "ro.system.build.version.incremental") == 0) {
        hooked_result = env->NewStringUTF("V14.0.6.0.UMCCNXM");
    } else if (strcmp(key, "ro.build.version.release") == 0) {
        hooked_result = env->NewStringUTF("14");
    } else if (strcmp(key, "ro.build.version.sdk") == 0 ||
               strcmp(key, "ro.build.version.sdk_int") == 0) {
        hooked_result = env->NewStringUTF("34");
    
    // ==== 8. 地区信息 ====
    } else if (strcmp(key, "ro.product.locale.region") == 0 ||
               strcmp(key, "ro.product.locale") == 0 ||
               strcmp(key, "persist.sys.locale") == 0) {
        hooked_result = env->NewStringUTF("CN");
    } else if (strcmp(key, "ro.product.mod_device") == 0) {
        hooked_result = env->NewStringUTF("fuxi_cn");
    
    // ==== 9. 硬件信息 ====
    } else if (strcmp(key, "ro.hardware") == 0 ||
               strcmp(key, "ro.boot.hardware") == 0 ||
               strcmp(key, "ro.hardware.platform") == 0) {
        hooked_result = env->NewStringUTF("qcom");
    } else if (strcmp(key, "ro.product.board") == 0 ||
               strcmp(key, "ro.board.platform") == 0) {
        hooked_result = env->NewStringUTF("kalama");
    
    // ==== 10. 构建类型和签名 ====
    } else if (strcmp(key, "ro.build.tags") == 0) {
        hooked_result = env->NewStringUTF("release-keys");
    } else if (strcmp(key, "ro.build.type") == 0) {
        hooked_result = env->NewStringUTF("user");
    } else if (strcmp(key, "ro.build.characteristics") == 0) {
        hooked_result = env->NewStringUTF("default,phone");
    } else if (strcmp(key, "ro.build.flavor") == 0) {
        hooked_result = env->NewStringUTF("fuxi-user");
    
    // ==== 11. 屏蔽AOSP/原生Android特征 ====
    } else if (strcmp(key, "ro.build.id") == 0 && 
               (strstr(def, "AOSP") || strstr(def, "aosp") || 
                strstr(def, "lineage") || strstr(def, "Lineage") ||
                strstr(def, "pixel") || strstr(def, "Pixel"))) {
        hooked_result = env->NewStringUTF("UKQ1.231117.002");
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

void setBuildIntField(JNIEnv *env, jclass clazz, const char *fieldName, int value) {
    if (clazz == nullptr || fieldName == nullptr) return;
    
    jfieldID fieldId = env->GetStaticFieldID(clazz, fieldName, "I");
    if (fieldId != nullptr) {
        env->SetStaticIntField(clazz, fieldId, value);
    }
}

// 屏蔽其他品牌Build字段
void maskOtherBrandFields(JNIEnv *env, jclass build_class) {
    // 可能的其他品牌字段
    const char* other_brand_fields[] = {
        "SUPPORTED_ABIS", "SUPPORTED_32_BIT_ABIS", "SUPPORTED_64_BIT_ABIS",
        "BOOTLOADER", "RADIO", "HARDWARE", "BOARD", "DEVICE", "PRODUCT"
    };
    
    // 确保这些字段不包含其他品牌特征
    jstring default_value = env->NewStringUTF("");
    for (const char* field : other_brand_fields) {
        jfieldID fieldId = env->GetStaticFieldID(build_class, field, "Ljava/lang/String;");
        if (fieldId != nullptr) {
            jstring current = (jstring)env->GetStaticObjectField(build_class, fieldId);
            if (current != nullptr) {
                const char* str = env->GetStringUTFChars(current, nullptr);
                // 如果包含其他品牌特征，清空或替换
                if (str && (strstr(str, "HUAWEI") || strstr(str, "huawei") ||
                           strstr(str, "OPPO") || strstr(str, "oppo") ||
                           strstr(str, "VIVO") || strstr(str, "vivo") ||
                           strstr(str, "SAMSUNG") || strstr(str, "samsung") ||
                           strstr(str, "ONEPLUS") || strstr(str, "oneplus") ||
                           strstr(str, "MEIZU") || strstr(str, "meizu") ||
                           strstr(str, "REALME") || strstr(str, "realme") ||
                           strstr(str, "GOOGLE") || strstr(str, "google") ||
                           strstr(str, "PIXEL") || strstr(str, "pixel"))) {
                    env->SetStaticObjectField(build_class, fieldId, default_value);
                }
                env->ReleaseStringUTFChars(current, str);
            }
            env->DeleteLocalRef(current);
        }
    }
    env->DeleteLocalRef(default_value);
}

void hookBuild(JNIEnv *env) {
    LOGD("hook Build - Xiaomi 13 (屏蔽其他品牌特征)\n");
    
    jclass build_class = env->FindClass("android/os/Build");
    jclass build_version_class = env->FindClass("android/os/Build$VERSION");
    
    if (build_class != nullptr) {
        // 先屏蔽其他品牌特征
        maskOtherBrandFields(env, build_class);
        
        // 设置小米特征
        setBuildField(env, build_class, "BRAND", "Xiaomi");
        setBuildField(env, build_class, "MANUFACTURER", "Xiaomi");
        setBuildField(env, build_class, "MODEL", "Xiaomi 13");
        setBuildField(env, build_class, "DEVICE", "fuxi");
        setBuildField(env, build_class, "PRODUCT", "fuxi");
        setBuildField(env, build_class, "BOARD", "kalama");
        setBuildField(env, build_class, "HARDWARE", "qcom");
        
        // 构建指纹
        setBuildField(env, build_class, "FINGERPRINT", 
            "Xiaomi/fuxi/fuxi:14/UKQ1.231117.002/V14.0.6.0.UMCCNXM:user/release-keys");
        
        // ID和序列号
        setBuildField(env, build_class, "ID", "UKQ1.231117.002");
        setBuildField(env, build_class, "SERIAL", "98KRNW4321A");
        
        env->DeleteLocalRef(build_class);
    }
    
    if (build_version_class != nullptr) {
        setBuildField(env, build_version_class, "RELEASE", "14");
        setBuildField(env, build_version_class, "INCREMENTAL", "V14.0.6.0.UMCCNXM");
        setBuildField(env, build_version_class, "SECURITY_PATCH", "2024-01-01");
        setBuildIntField(env, build_version_class, "SDK_INT", 34);
        
        env->DeleteLocalRef(build_version_class);
    }
    
    LOGD("hook Build done (已屏蔽其他品牌特征)");
}

void hookSystemProperties(JNIEnv *env, zygisk::Api *api) {
    LOGD("hook SystemProperties - 屏蔽其他品牌特征\n");

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
