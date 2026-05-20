FROM ghcr.io/userver-framework/ubuntu-22.04-userver-pg-dev:latest

RUN apt-get update && \
    apt-get install -y \
    wget \
    libssl-dev \
    librabbitmq-dev \
    git \
    nlohmann-json3-dev \
    python3 \
    python3-pip \
    && rm -rf /var/lib/apt/lists/* && \
    # Python testing dependencies installation
    pip3 install --no-cache-dir \
    pytest \
    pytest-asyncio \
    PyJWT \
    aiohttp && \
    # JWT-CPP installation
    cd /tmp && \
    git clone --depth 1 --branch v0.7.0 https://github.com/Thalhammer/jwt-cpp.git && \
    cd jwt-cpp && \
    mkdir -p build && \
    cd build && \
    cmake -DCMAKE_BUILD_TYPE=Release \
          -DJWT_BUILD_EXAMPLES=OFF \
          -DJWT_BUILD_TESTS=OFF \
          -DJWT_INSTALL=ON \
          -DCMAKE_INSTALL_PREFIX=/usr/local \
          -DDEFAULT_NLOHMANN_JSON_BACKEND=OFF \
          .. && \
    make install && \
    ldconfig && \
    rm -rf /tmp/jwt-cpp

WORKDIR /app
COPY . /app

RUN mkdir -p build && \
    cd build && \
    cmake -DCMAKE_BUILD_TYPE=Release \
          -DUSERVER_FEATURE_POSTGRESQL=ON \
          -DUSERVER_FEATURE_TESTSUITE=OFF \
          -DUSERVER_FEATURE_FORMAT=OFF \
          .. && \
    make -j2

RUN mkdir -p /app/data && \
    cp /app/build/delivery-service-user /app/delivery-service-user && \
    cp /app/build/delivery-service-parcel /app/delivery-service-parcel && \
    cp /app/build/delivery-service-delivery /app/delivery-service-delivery && \
    chmod +x /app/delivery-service-user \
    && chmod +x /app/delivery-service-parcel \
    && chmod +x /app/delivery-service-delivery

# Default command, will be overridden in docker compose
CMD ["/bin/bash"]