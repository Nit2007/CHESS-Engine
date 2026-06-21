FROM eclipse-temurin:21-jdk-jammy

# Install Maven and C++ build tools
RUN apt-get update && apt-get install -y \
    maven \
    build-essential \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

# Copy the entire project so we can access ../src
COPY . .

WORKDIR /app/backend

# Build the native library and the Spring Boot application
RUN mvn clean package -DskipTests -Dnative.build=true

# Expose the port (Render sets PORT env variable dynamically)
EXPOSE 8080

CMD ["java", "-Xmx256m", "-XX:+UseSerialGC", "-jar", "target/CHESS-Engine-Backend-0.0.1-SNAPSHOT.jar"]
