package com.sarun_2_7.CHESS_Engine_Backend.service.jni;

import java.io.File;
import java.io.IOException;
import java.io.InputStream;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.StandardCopyOption;

/**
 * NativeLibLoader extracts a packaged native library from classpath
 * `resources/native/` to a temporary file and loads it via System.load().
 *
 * Purpose: avoid requiring users to configure java.library.path manually
 * and to allow the Spring Boot jar to contain the native library.
 */
public final class NativeLibLoader {
    private NativeLibLoader() {}

    public static void load(String baseName) {
        String mappedName = mapLibraryName(baseName);
        String resourcePath = "/native/" + mappedName;

        try (InputStream in = NativeLibLoader.class.getResourceAsStream(resourcePath)) {
            if (in == null) {
                throw new UnsatisfiedLinkError("Native library not found in resources: " + resourcePath);
            }

            // Write to a temp file and load from there
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
