/*
 * Copyright (C) 2013 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.example.android.bluetoothlegatt;

import java.util.HashMap;

/**
 * This class includes a small subset of standard GATT attributes for demonstration purposes.
 */
public class SampleGattAttributes {
    private static HashMap<String, String> attributes = new HashMap();
    //0000XXXX-0000-1000-8000-00805f9b34fb
    public static String HEART_RATE_MEASUREMENT = "19B10000-E8F2-537E-4F6C-D104768A1214";
    public static String CLIENT_CHARACTERISTIC_CONFIG = "00002902-0000-1000-8000-00805f9b34fb";

    static {
        // Sample Services.
        attributes.put("19B10000-E8F2-537E-4F6C-D104768A1214", "Heart Rate Service");
        //attributes.put("0000180d-0000-1000-8000-00805f9b34fb", "Device Information Service");
        // Sample Characteristics.
        //00002a29-0000-1000-8000-00805f9b34fb
        attributes.put(HEART_RATE_MEASUREMENT, "Heart Rate Measurement");
        attributes.put(CLIENT_CHARACTERISTIC_CONFIG,"Client Characteristic");
        attributes.put("19B10001-E8F2-537E-4F6C-D104768A1214", "Manufacturer Name String");
        attributes.put("00002a29-0000-1000-8000-00805f9b34fb","basic");
    }

    public static String lookup(String uuid, String defaultName) {
        String name = attributes.get(uuid);
        return name == null ? defaultName : name;
    }
}
