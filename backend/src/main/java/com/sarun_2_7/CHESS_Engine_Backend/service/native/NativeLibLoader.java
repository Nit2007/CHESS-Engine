package com.sarun_2_7.CHESS_Engine_Backend.service.jni;

import java.io.File;
import java.io.IOException;
import java.io.InputStream;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.StandardCopyOption;

/**
 * NativeLibLoader loads the native chess engine library.
 *
 * Strategy (in priority order):
 *   1. Load directly from the C++ build output at {@code src/native/} —
 *      this avoids the need to copy the DLL after every rebuild.
 *   2. Fallback: extract from classpath {@code resources/native/} for
 *      packaged/deployed scenarios.
 */
public final class NativeLibLoader {
    private NativeLibLoader() {}

    /**
     * Relative path to the C++ build output directory.
     * Resolved from the backend's working directory (backend/).
     * backend/ → ../src/native/
     */
    private static final String SRC_NATIVE_DIR = "../src/native";

    public static void load(String baseName) {
        String mappedName = mapLibraryName(baseName);

        // --- Strategy 1: load directly from the src build output ---
        File srcLib = new File(SRC_NATIVE_DIR, mappedName);
        if (srcLib.isFile()) {
            System.out.println("[NativeLibLoader] Loading from src: " + srcLib.getAbsolutePath());
            System.load(srcLib.getAbsolutePath());
            return;
        }

        // --- Strategy 2: fallback to classpath resources ---
        String resourcePath = "/native/" + mappedName;
        System.out.println("[NativeLibLoader] src lib not found, falling back to classpath: " + resourcePath);

        try (InputStream in = NativeLibLoader.class.getResourceAsStream(resourcePath)) {
            if (in == null) {
                throw new UnsatisfiedLinkError(
                        "Native library not found at " + srcLib.getAbsolutePath()
                        + " nor in classpath resources: " + resourcePath);
            }

            Path tempDir = Files.createTempDirectory("chess_native_");
            tempDir.toFile().deleteOnExit();

            Path libFile = tempDir.resolve(mappedName);
            Files.copy(in, libFile, StandardCopyOption.REPLACE_EXISTING);
            libFile.toFile().deleteOnExit();

            System.load(libFile.toAbsolutePath().toString());
        } catch (IOException e) {
            throw new UnsatisfiedLinkError("Failed to extract native library: " + e.getMessage());
        }
    }

    private static String mapLibraryName(String baseName) {
        String os = System.getProperty("os.name").toLowerCase();
        if (os.contains("win")) {
            return baseName + ".dll";
        } else if (os.contains("mac")) {
            return "lib" + baseName + ".dylib";
        } else {
            return "lib" + baseName + ".so";
        }
    }
}
