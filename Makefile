# Compiler
CC = g++
# OPTS = -Wall -Werror -Wunused -O2 -g
OPTS = -Wall -O2 -g \
       -Wmissing-declarations -Wreturn-type -Wunused \
       -Wcomment -Wformat

# Project name
PROJECT = muon_g2_nmr

# Directories
OBJDIR = obj
SRCDIR = src

# Libraries
LIBS = -lm

# Files and folders
SRCS    = $(shell find $(SRCDIR) -name '*.c')
SRCDIRS = $(shell find $(SRCDIR) -type d | sed 's/$(SRCDIR)/./g' )
OBJS    = $(patsubst $(SRCDIR)/%.c,$(OBJDIR)/%.o,$(SRCS))

# Targets
$(PROJECT): buildrepo $(OBJS)
	$(CC) $(OBJS) $(LIBS) -o $@

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(OPTS) -c $< -o $@

.PHONY: clean
clean:
	rm -Rf $(PROJECT) $(OBJDIR) $(SRCDIR)/*~  *~ 

buildrepo:
	@$(call make-repo)

# Create obj directory structure
define make-repo
	mkdir -p $(OBJDIR)
	for dir in $(SRCDIRS); \
	do \
		mkdir -p $(OBJDIR)/$$dir; \
	done
endef

