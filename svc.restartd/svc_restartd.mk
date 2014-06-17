##
## Auto Generated makefile by CodeLite IDE
## any manual changes will be erased      
##
## Debug
ProjectName            :=svc_restartd
ConfigurationName      :=Debug
WorkspacePath          := "/ws/svc.restartd"
ProjectPath            := "/ws/svc.restartd"
IntermediateDirectory  :=./Debug
OutDir                 := $(IntermediateDirectory)
CurrentFileName        :=
CurrentFilePath        :=
CurrentFileFullPath    :=
User                   :=David Mackay
Date                   :=06/17/14
CodeLitePath           :="/export/home/david/.codelite"
LinkerName             :=clang
SharedObjectLinkerName :=clang -shared -fPIC
ObjectSuffix           :=.o
DependSuffix           :=
PreprocessSuffix       :=.o.i
DebugSwitch            :=-gstab
IncludeSwitch          :=-I
LibrarySwitch          :=-l
OutputSwitch           :=-o 
LibraryPathSwitch      :=-L
PreprocessorSwitch     :=-D
SourceSwitch           :=-c 
OutputFile             :=$(IntermediateDirectory)/$(ProjectName)
Preprocessors          :=
ObjectSwitch           :=-o 
ArchiveOutputSwitch    := 
PreprocessOnlySwitch   :=-E 
ObjectsFileList        :="svc_restartd.txt"
PCHCompileFlags        :=
MakeDirCommand         :=mkdir -p
LinkOptions            :=  
IncludePath            :=  $(IncludeSwitch). $(IncludeSwitch). 
IncludePCH             := 
RcIncludePath          := 
Libs                   := 
ArLibs                 :=  
LibPath                := $(LibraryPathSwitch). 

##
## Common variables
## AR, CXX, CC, AS, CXXFLAGS and CFLAGS can be overriden using an environment variables
##
AR       := ar rcus
CXX      := clang++
CC       := clang
CXXFLAGS :=  -g -O0 -Wall $(Preprocessors)
CFLAGS   :=  -g -O0 -Wall $(Preprocessors)
ASFLAGS  := 
AS       := llvm-as


##
## User defined environment variables
##
CodeLiteDir:=/usr/local/share/codelite
Objects0=$(IntermediateDirectory)/util$(ObjectSuffix) $(IntermediateDirectory)/pidlist$(ObjectSuffix) $(IntermediateDirectory)/config$(ObjectSuffix) $(IntermediateDirectory)/main$(ObjectSuffix) 



Objects=$(Objects0) 

##
## Main Build Targets 
##
.PHONY: all clean PreBuild PrePreBuild PostBuild
all: $(OutputFile)

$(OutputFile): $(IntermediateDirectory)/.d $(Objects) 
	@$(MakeDirCommand) $(@D)
	@echo "" > $(IntermediateDirectory)/.d
	@echo $(Objects0)  > $(ObjectsFileList)
	$(LinkerName) $(OutputSwitch)$(OutputFile) @$(ObjectsFileList) $(LibPath) $(Libs) $(LinkOptions)

$(IntermediateDirectory)/.d:
	@test -d ./Debug || $(MakeDirCommand) ./Debug

PreBuild:


##
## Objects
##
$(IntermediateDirectory)/util$(ObjectSuffix): util.c 
	$(CC) $(SourceSwitch) "/ws/svc.restartd/util.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/util$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/util$(PreprocessSuffix): util.c
	@$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/util$(PreprocessSuffix) "util.c"

$(IntermediateDirectory)/pidlist$(ObjectSuffix): pidlist.c 
	$(CC) $(SourceSwitch) "/ws/svc.restartd/pidlist.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/pidlist$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/pidlist$(PreprocessSuffix): pidlist.c
	@$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/pidlist$(PreprocessSuffix) "pidlist.c"

$(IntermediateDirectory)/config$(ObjectSuffix): config.c 
	$(CC) $(SourceSwitch) "/ws/svc.restartd/config.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/config$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/config$(PreprocessSuffix): config.c
	@$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/config$(PreprocessSuffix) "config.c"

$(IntermediateDirectory)/main$(ObjectSuffix): main.c 
	$(CC) $(SourceSwitch) "/ws/svc.restartd/main.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/main$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/main$(PreprocessSuffix): main.c
	@$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/main$(PreprocessSuffix) "main.c"

##
## Clean
##
clean:
	$(RM) $(IntermediateDirectory)/util$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/util$(DependSuffix)
	$(RM) $(IntermediateDirectory)/util$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/pidlist$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/pidlist$(DependSuffix)
	$(RM) $(IntermediateDirectory)/pidlist$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/config$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/config$(DependSuffix)
	$(RM) $(IntermediateDirectory)/config$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/main$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/main$(DependSuffix)
	$(RM) $(IntermediateDirectory)/main$(PreprocessSuffix)
	$(RM) $(OutputFile)
	$(RM) ".build-debug/svc_restartd"


