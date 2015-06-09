#
# rainbow.mk
#
#  Created on: Aug 21, 2011
#  Author:     Brian Yi ZHANG
#  Email:      brianlions@gmail.com
#

# 30 black
# 31 red
# 32 green
# 33 yellow
# 34 blue
# 35 magenta
# 36 cyan
# 37 white
# 0; normal display, 1; to highlight
ifndef NO_COLOR
PROTOC_COLOR_A="\033[1;35m"
PROTOC_COLOR_B="\033[1;35m"
DEP_COLOR_A="\033[1;34m"
DEP_COLOR_B="\033[1;34m"
OBJ_COLOR_A="\033[1;36m"
OBJ_COLOR_B="\033[1;36m"
EXE_COLOR_A="\033[1;32m"
EXE_COLOR_B="\033[1;32m"
AR_COLOR_A ="\033[1;35m"
AR_COLOR_B ="\033[1;35m"
END_COLOR  ="\033[0m"
COLOR_DEP  =@printf '%b\t%b\n' $(DEP_COLOR_A)CPP$(END_COLOR) $(DEP_COLOR_B)$@$(END_COLOR) &&
COLOR_OBJ  =@printf '%b\t%b\n' $(OBJ_COLOR_A)CC$(END_COLOR) $(OBJ_COLOR_B)$@$(END_COLOR) &&
COLOR_EXE  =@printf '%b\t%b\n' $(EXE_COLOR_A)LINK$(END_COLOR) $(EXE_COLOR_B)$@$(END_COLOR) &&
COLOR_AR   =@printf '%b\t%b\n' $(AR_COLOR_A)AR$(END_COLOR) $(AR_COLOR_B)$@$(END_COLOR) &&
COLOR_PROTOC   =@printf '%b\t%b\n' $(PROTOC_COLOR_A)PROTOC$(END_COLOR) $(PROTOC_COLOR_B)$@$(END_COLOR) &&
COLORFUL='yes'
else
COLOR_DEP=
COLOR_OBJ=
COLOR_EXE=
COLOR_AR=
COLOR_PROTOC=
COLORFUL=
endif
