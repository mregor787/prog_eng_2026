FROM ghcr.io/userver-framework/ubuntu-22.04-userver-pg-dev:latest

WORKDIR /app
COPY . .

RUN cmake -B build -DCMAKE_BUILD_TYPE=Release \
    -DUSERVER_FEATURE_TESTSUITE=OFF \
    -DUSERVER_FEATURE_POSTGRESQL=OFF

RUN cmake --build build -j

CMD ["./build/delivery_service", "--config", "configs/static_config.yaml"]