# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++11 -Wall -O3 -shared -fPIC `python3 -m pybind11 --includes`

# Define the folder containing images
IMG_FOLDER := img_result
FILE_FOLDER := src
IMG_PREFIX := voronoi_plot_num=

# Targets
TARGET_FORTUNE = $(FILE_FOLDER)/_voronoi_fortune.so
TARGET_HALFPLANE = $(FILE_FOLDER)/_voronoi_halfplane.so

# Source files (located in src folder)
SRCS_FORTUNE = $(FILE_FOLDER)/voronoi_fortune.cc $(FILE_FOLDER)/voronoi_common.hh
SRCS_HALFPLANE = $(FILE_FOLDER)/voronoi_bruteforce.cc $(FILE_FOLDER)/voronoi_common.hh

# Python include directory
PYTHON_INCLUDE = $(shell python3-config --includes)

# Build targets
all: $(TARGET_FORTUNE) $(TARGET_HALFPLANE)

$(TARGET_FORTUNE): $(SRCS_FORTUNE)
	$(CXX) $(CXXFLAGS) $(SRCS_FORTUNE) -o $(TARGET_FORTUNE)

$(TARGET_HALFPLANE): $(SRCS_HALFPLANE)
	$(CXX) $(CXXFLAGS) $(SRCS_HALFPLANE) -o $(TARGET_HALFPLANE)



# Targets for testing and plotting
test: $(TARGET_FORTUNE) $(TARGET_HALFPLANE)
	python3 -m pytest -v $(FILE_FOLDER)/test_voronoi.py

plot_graph: $(TARGET_FORTUNE) $(TARGET_HALFPLANE)
	python3 $(FILE_FOLDER)/plot_graph.py

plot_performance: $(TARGET_FORTUNE) $(TARGET_HALFPLANE)
	python3 $(FILE_FOLDER)/plot_performance.py

performance: $(TARGET_FORTUNE) $(TARGET_HALFPLANE)
	python3 $(FILE_FOLDER)/performance.py

# Define the clean_voronoi_graph target
clean_voronoi_graph:
	@echo "Removing Voronoi plot images from $(IMG_FOLDER)..."
	@rm -f $(IMG_FOLDER)/$(IMG_PREFIX)*.png
	@echo "Cleanup complete!"

# Clean target (removes compiled .so files inside the src folder)
clean:
	rm -f $(FILE_FOLDER)/*.so $(FILE_FOLDER)/*.o

clean_txt:
	rm -rf $(FILE_FOLDER)/*.txt

clean_test:
	rm -rf $(FILE_FOLDER)/__pycache__ $(FILE_FOLDER)/.pytest_cache