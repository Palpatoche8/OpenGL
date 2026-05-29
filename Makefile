CC := g++
CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17 -O3 -fopenmp
VCPKG = D:/ING2/vcpkg
VCPKG_LIB = $(VCPKG)/installed/x64-mingw-static/lib
VCPKG_INC = $(VCPKG)/installed/x64-mingw-static/include

GLAD_INC := extern/glad/include
GLAD_SRC := extern/glad/src/glad.c
GLAD_OBJ := build/glad.o


ifeq ($(OS), Windows_NT)
	CXXFLAGS += -I"$(VCPKG_INC)"
	LIBS = -L"$(VCPKG_LIB)" -lglfw3 -lopengl32 -lgdi32 -luser32 -lshell32 -static -fopenmp
else
	LIBS = -lglfw -lGL -lXrandr -lXxf86vm -lXi -lXinerama -lX11 -lrt -ldl -pthread -fopenmp
endif

INCS     := -I"$(VCPKG_INC)" -I$(GLAD_INC) -Isrc -Iextern/glm

TARGET   := build/fire_sim
SRCS     := src/main.cpp src/Shader.cpp src/ParticleSystem.cpp
OBJS     := $(patsubst src/%.cpp, build/%.o, $(SRCS))

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS) $(GLAD_OBJ) | build
	$(CXX) $(CXXFLAGS) $^ $(LIBS) -o $@
	xcopy /E /I /Y shaders build\shaders
	
build/%.o: src/%.cpp | build
	$(CXX) $(CXXFLAGS) $(INCS) -c $< -o $@

$(GLAD_OBJ): $(GLAD_SRC) | build
	$(CC) $(CFLAGS) $(INCS) -c $< -o $@

build:
	mkdir -p build

clean:
	rm -rf build
