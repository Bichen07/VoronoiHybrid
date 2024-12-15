# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++11 -Wall -O3 -shared -fPIC `python3 -m pybind11 --includes`

# Targets
TARGET_FORTUNE = _voronoi_fortune.so
TARGET_HALFPLANE = _voronoi_halfplane.so

# Source files
SRCS_FORTUNE = voronoi_fortune.cc voronoi_common.hh
SRCS_HALFPLANE = voronoi_bruteforce.cc voronoi_common.hh

# Python include directory
PYTHON_INCLUDE = $(shell python3-config --includes)

# Build targets
all: $(TARGET_FORTUNE) $(TARGET_HALFPLANE)

$(TARGET_FORTUNE): $(SRCS_FORTUNE)
	$(CXX) $(CXXFLAGS) $(SRCS_FORTUNE) -o $(TARGET_FORTUNE)

$(TARGET_HALFPLANE): $(SRCS_HALFPLANE)
	$(CXX) $(CXXFLAGS) $(SRCS_HALFPLANE) -o $(TARGET_HALFPLANE)

# Define the folder containing images
IMG_FOLDER := img_result
IMG_PREFIX := voronoi_plot_num=

test: $(TARGET)
	python3 -m pytest -v test_voronoi.py

plot_graph: $(TARGET)
	python3 plot_graph.py

plot_performance: $(TARGET)
	python3 plot_performance.py

performance: $(TARGET)
	python3 performance.py


# Define the clean_voronoi_graph target
clean_voronoi_graph:
	@echo "Removing Voronoi plot images from $(IMG_FOLDER)..."
	@rm -f $(IMG_FOLDER)/$(IMG_PREFIX)*.png
	@echo "Cleanup complete!"

# Clean target
clean:
	rm -f $(TARGET) *.o *.so

clean_txt:
	rm -rf *.txt

clean_test:
	rm -rf __pycache__ .pytest_cache