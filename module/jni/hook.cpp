#include "hook.h"
#include "logging.h"
#include <string>

using namespace std;

jstring (*orig_native_get)(JNIEnv *env, jclass clazz, jstring keyJ, jstring defJ);

jstring my_native_get(JNIEnv *env, jclass clazz, jstring keyJ, jstring defJ) {
    const char *key = env->GetStringUTFChars(keyJ, nullptr);
    const char *def = env->GetStringUTFChars(defJ, nullptr);

    jstring hooked_result = nullptr;

    // 品牌和制造商（最关键）
    if (strcmp(key, "ro.product.brand") == 0) {
        hooked_result = env->NewStringUTF("Xiaomi");
    } else if (strcmp(key, "ro.product.manufacturer") == 0) {
        hooked_result = env->NewStringUTF("Xiaomi");
    } 
    // 型号 - 使用用户友好名称而非内部代码
    else if (strcmp(key, "ro.product.model") == 0 ||
             strcmp(key, "ro.product.system.model") == 0) {
        hooked_result = env->NewStringUTF("Xiaomi 14");
    }
    // 设备标识
    else if (strcmp(key, "ro.product.device") == 0) {
        hooked_result = env->NewStringUTF("shennong");
    } else if (strcmp(key, "ro.product.name") == 0) {
        hooked_result = env->NewStringUTF("shennong");
    }
    // MIUI版本信息 - MiPush必备
    else if (strcmp(key, "ro.miui.ui.version.code") == 0) {
        hooked_result = env->NewStringUTF("14");
    } else if (strcmp(key, "ro.miui.ui.version.name") == 0) {
        hooked_result = env->NewStringUTF("V14");
    } else if (strcmp(key, "ro.miui.version.code_time") == 0) {
        hooked_result = env->NewStringUTF("230618");
    } else if (strcmp(key, "ro.build.version.incremental") == 0) {
        hooked_result = env->NewStringUTF("V14.0.6.0.UMCCNXM");
    }
    // 构建信息 - 提高伪装可信度
    else if (strcmp(key, "ro.build.fingerprint") == 0) {
        hooked_result = env->NewStringUTF("Xiaomi/shennong/shennong:14/UKQ1.231117.002/V14.0.6.0.UMCCNXM:user/release-keys");
    } else if (strcmp(key, "ro.build.version.release") == 0) {
        hooked_result = env->NewStringUTF("14");
    } else if (strcmp(key, "ro.build.version.sdk") == 0) {
        hooked_result = env->NewStringUTF("34");
    }
    // 区域信息 - 区分国行/国际版
    else if (strcmp(key, "ro.product.locale.region") == 0) {
        hooked_result = env->NewStringUTF("CN");
    } else if (strcmp(key, "ro.product.mod_device") == 0) {
        hooked_result = env->NewStringUTF("shennong_cn");
    }
    // MIUI特有功能标识
    else if (strcmp(key, "ro.miui.has_handsfree_mode") == 0 ||
             strcmp(key, "ro.miui.has_real_blur") == 0) {
        hooked_result = env->NewStringUTF("true");
    }

    env->ReleaseStringUTFChars(keyJ, key);
    env->ReleaseStringUTFChars(defJ, def);

    if (hooked_result != nullptr) {
        return hooked_result;
    } else {
        return orig_native_get(env, clazz, keyJ, defJ);
    }
}

void hookBuild(JNIEnv *env) {
    LOGD("hook Build for MiPush\n");
    jclass build_class = env->FindClass("android/os/Build");
    
    // 核心字段
    jstring new_brand = env->NewStringUTF("Xiaomi");
    jstring new_manufacturer = env->NewStringUTF("Xiaomi");
    jstring new_model = env->NewStringUTF("Xiaomi 14");  // 使用友好名称
    jstring new_device = env->NewStringUTF("shennong");
    jstring new_product = env->NewStringUTF("shennong");
    
    // 增强伪装字段
    jstring new_fingerprint = env->NewStringUTF("Xiaomi/shennong/shennong:14/UKQ1.231117.002/V14.0.6.0.UMCCNXM:user/release-keys");
    jstring new_board = env->NewStringUTF("kalama");
    
    // 修改 BRAND（最关键）
    jfieldID brand_id = env->GetStaticFieldID(build_class, "BRAND", "Ljava/lang/String;");
    if (brand_id != nullptr) {
        env->SetStaticObjectField(build_class, brand_id, new_brand);
    }

    // 修改 MANUFACTURER（最关键）
    jfieldID manufacturer_id = env->GetStaticFieldID(build_class, "MANUFACTURER", "Ljava/lang/String;");
    if (manufacturer_id != nullptr) {
        env->SetStaticObjectField(build_class, manufacturer_id, new_manufacturer);
    }
    
    // 修改 MODEL（使用友好名称）
    jfieldID model_id = env->GetStaticFieldID(build_class, "MODEL", "Ljava/lang/String;");
    if (model_id != nullptr) {
        env->SetStaticObjectField(build_class, model_id, new_model);
    }
    
    // 修改 DEVICE
    jfieldID device_id = env->GetStaticFieldID(build_class, "DEVICE", "Ljava/lang/String;");
    if (device_id != nullptr) {
        env->SetStaticObjectField(build_class, device_id, new_device);
    }
    
    // 修改 PRODUCT
    jfieldID product_id = env->GetStaticFieldID(build_class, "PRODUCT", "Ljava/lang/String;");
    if (product_id != nullptr) {
        env->SetStaticObjectField(build_class, product_id, new_product);
    }
    
    // 修改 FINGERPRINT（增强伪装）
    jfieldID fingerprint_id = env->GetStaticFieldID(build_class, "FINGERPRINT", "Ljava/lang/String;");
    if (fingerprint_id != nullptr) {
        env->SetStaticObjectField(build_class, fingerprint_id, new_fingerprint);
    }
    
    // 修改 BOARD（可选，增加真实感）
    jfieldID board_id = env->GetStaticFieldID(build_class, "BOARD", "Ljava/lang/String;");
    if (board_id != nullptr) {
        env->SetStaticObjectField(build_class, board_id, new_board);
    }

    env->DeleteLocalRef(new_brand);
    env->DeleteLocalRef(new_manufacturer);
    env->DeleteLocalRef(new_model);
    env->DeleteLocalRef(new_device);
    env->DeleteLocalRef(new_product);
    env->DeleteLocalRef(new_fingerprint);
    env->DeleteLocalRef(new_board);

    LOGD("hook Build for MiPush done");
}

void hookSystemProperties(JNIEnv *env, zygisk::Api *api) {
    LOGD("hook SystemProperties for MiPush\n");

    JNINativeMethod targetHookMethods[] = {
            {"native_get", "(Ljava/lang/String;Ljava/lang/String;)Ljava/lang/String;",
             (void *) my_native_get},
    };

    api->hookJniNativeMethods(env, "android/os/SystemProperties", targetHookMethods, 1);

    *(void **) &orig_native_get = targetHookMethods[0].fnPtr;

    LOGD("hook SystemProperties done: %p\n", orig_native_get);
}

void Hook::hook() {
    hookBuild(env);
    hookSystemProperties(env, api);
}
