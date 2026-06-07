CC  := g++
CXX := g++
CXXFLAGS := -Wall -Wextra -std=c++17 -O3 -fopenmp

VCPKG     := C:/Users/Utilisateur/Desktop/ING2/Synthese Image/Projet/vcpkg
VCPKG_LIB := $(VCPKG)/installed/x64-mingw-static/lib
VCPKG_INC := $(VCPKG)/installed/x64-mingw-static/include

GLAD_INC := extern/glad/include
GLAD_SRC := extern/glad/src/glad.c
GLAD_OBJ := build/glad.o

ifeq ($(OS), Windows_NT)
CXXFLAGS += -I"$(VCPKG_INC)"
LIBS      = -L"$(VCPKG_LIB)" -lglfw3 -lopengl32 -lgdi32 -luser32 -lshell32 -static -fopenmp
COPY_CMD  = xcopy /E /I /Y shaders build\shaders & if exist models xcopy /E /I /Y models build\models
else
LIBS      = -lglfw -lGL -ldl -pthread -fopenmp
COPY_CMD  = cp -r shaders build/ && ([ -d models ] && cp -r models build/ || true)
endif

INCS := -I"$(VCPKG_INC)" -I$(GLAD_INC) -Isrc

TARGET := build/fire_sim
SRCS   := src/main.cpp           \
           src/Shader.cpp        \
           src/ParticleSystem.cpp \
           src/SparkSystem.cpp   \
           src/SmokeSystem.cpp   \
           src/Skybox.cpp        \
           src/Mesh.cpp
OBJS   := $(patsubst src/%.cpp, build/%.o, $(SRCS))

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS) $(GLAD_OBJ) | build
	$(CXX) $(CXXFLAGS) $^ $(LIBS) -o $@
	$(COPY_CMD)

build/%.o: src/%.cpp | build
	$(CXX) $(CXXFLAGS) $(INCS) -c $< -o $@

$(GLAD_OBJ): $(GLAD_SRC) | build
	$(CC) $(INCS) -c $< -o $@

build:
	mkdir -p build

clean:
	rm -rf build
