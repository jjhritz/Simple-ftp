
# Automatically generated on Tue, Sep 27 2016 19:27:06

DEP = .deps
CC  = gcc
CXX = g++
CFLAGS   = -Wall 
CXXFLAGS = -Wall 

# Target
SOURCES  = client.cpp
OBJECTS  = client.o
DEPFILES = $(patsubst %.o,$(DEP)/%.d,$(OBJECTS))
TARGET   = client
ZIP_FILE = source.zip

.PHONY: all clean zip

all: $(DEP) $(TARGET)

-include $(DEPFILES)

$(TARGET): $(OBJECTS)
	@echo "Linking $@"
	$(CXX) $(OBJECTS) -o $@ $(LIBS)

%.o: %.c
	@echo "Compiling $*.c"
	$(CC) -c $(CFLAGS) $*.c $(INCLUDE) -o $@
	@$(CC) -MM -MP -MT $@ $(CFLAGS) $*.c $(INCLUDE) > $(DEP)/$*.d

%.o: %.cpp
	@echo "Compiling $*.cpp"
	$(CXX) -c $(CXXFLAGS) $*.cpp $(INCLUDE) -o $@
	@$(CXX) -MM -MP -MT $@ $(CXXFLAGS) $*.cpp $(INCLUDE) > $(DEP)/$*.d

%.o: %.cc
	@echo "Compiling $*.cc"
	$(CXX) -c $(CXXFLAGS) $*.cc $(INCLUDE) -o $@
	@$(CXX) -MM -MP -MT $@ $(CXXFLAGS) $*.cc $(INCLUDE) > $(DEP)/$*.d

$(DEP):
	@mkdir -p $@

zip: $(SOURCES)
	@echo "Zipping source files to $(ZIP_FILE)"
	@zip $(ZIP_FILE) Makefile $(SOURCES) 

clean:
	@rm -f $(TARGET) $(DEPFILES) $(OBJECTS) $(ZIP_FILE)
	@rmdir $(DEP)


