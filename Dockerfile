# Stage 1: Build the compiler
FROM node:20-slim AS builder

RUN apt-get update && apt-get install -y clang make g++

WORKDIR /build
COPY Makefile .
COPY src ./src

# Build the compiler (produces /build/rox)
RUN make

# Stage 2: Runtime environment
FROM node:20-slim

# Install runtime dependencies (clang is needed because 'rox' invokes it)
RUN apt-get update && apt-get install -y clang && rm -rf /var/lib/apt/lists/*

WORKDIR /app

# Copy only the compiled binary from the builder stage
COPY --from=builder /build/rox .

# Copy web server
COPY web ./web

# Install web dependencies
WORKDIR /app/web
RUN npm install

# Expose port
EXPOSE 3000

# Start server (must run from /app/web so it finds ../rox)
CMD ["node", "server.js"]
