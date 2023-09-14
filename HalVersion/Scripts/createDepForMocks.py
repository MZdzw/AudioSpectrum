import os
import sys
import re

#### C-like structs

# class MyStruct():
#     def __init__(self, field1, field2, field3):
#         self.field1 = field1
#         self.field2 = field2
#         self.field3 = field3

class DataType():
    def __init__(self, dataName, startLine, endLine, type, predecesor, filename):
        self.dataName = dataName
        self.startLine = startLine
        self.endLine = endLine
        self.type = type
        self.predecesor = predecesor
        self.filename = filename
        self.fullyKnown = False
        self.insideStruct = []
        self.level = 0

class StructContent():
    def __init__(self):
        self.insideStruct = []
        self.parent = ''

        

def SearchFunctionDeclarations(content):
    matchingFunctions = []
    functionsParameters = []

    usedDataTypes = []
    

    for line in content:
        pattern = rf'.+\(.*\);'
        patternFunctionName = rf'\w\s*(.*?)\(.*\);'
        patternFunctionParams = r'\((.*?)\)'
        if re.match(pattern, line):
            matchingFunctions.append(re.findall(patternFunctionName, line))
            functionsParameters.append((re.findall(patternFunctionParams, line))[0].split(","))
            for i, s in enumerate(functionsParameters[-1]):
                functionsParameters[-1][i] = s.strip()
                usedDataTypes.append(functionsParameters[-1][i].split(" "))

    return usedDataTypes

def LookForNotKnownDataTypes(fname, existingNotKnownDataTypes):
    listOfNotKnownDataTypes = []
    #print(fname)

    fileHALDriver = open(fname, 'r')
    contentHALDriver = fileHALDriver.readlines()

    lineCounter = 1
    encounteredLine = 1
    for line in contentHALDriver:
        for x in existingNotKnownDataTypes:
            if re.match('}\s*' + rf'{re.escape(x.dataName)}', line):
                #print(str(lineCounter) + " - " + x + " - " + fname)
                encounteredLine = lineCounter

                cnt = 1
                while(cnt >= 1):
                    lineCounter -= 1
                    if (lineCounter <= 0):
                        break
                    if (re.match('}', contentHALDriver[lineCounter - 1])): 
                        cnt += 1
                    if (re.match('{', contentHALDriver[lineCounter - 1])): 
                        cnt -= 1
                    #print(contentHALDriver[lineCounter] + ' -- ' + str(cnt))

                regexType = contentHALDriver[lineCounter - 2].split(" ")
                listOfNotKnownDataTypes.append(DataType(x.dataName, lineCounter - 1, encounteredLine, regexType[1].rstrip(), x.predecesor, fname))
                lineCounter = encounteredLine

        lineCounter += 1
    #print(fname)

    fileHALDriver.close()
    return listOfNotKnownDataTypes

def GetNotKnownDataTypes(usedDataTypes):
    dataTypes = ["void", "uint8_t", "uint16_t", "uint32_t", "int8_t",
                 "int16_t", "int32_t", "char", "string", "unsigned char", "int",
                 "unsigned int", "short", "unsigned short", "long", "unsigned long",
                 "long long", "float", "double", "size_t", "UNITY_LINE_TYPE"]
    
    notKnownDataTypes = []
    for lineFunParams in usedDataTypes:
        if (lineFunParams[0][len(lineFunParams[0]) - 1] == '*'):
            # it's probably pointer
            lineFunParams[0] = lineFunParams[0][:-1]
        known = False
        for s in dataTypes:
            if (s == lineFunParams[0]):
                known = True
                break
        for x in notKnownDataTypes:
            if (x.dataName == lineFunParams[0]):
                known = True
                break
        
        if (known == False):
            notKnownDataTypes.append(DataType(lineFunParams[0], 0, 0, 'None', lineFunParams[0], 'None'))
    
    return notKnownDataTypes

def GetNotKnownDataTypesFromStruct(usedDataTypes, notKnownDataTypes):
    dataTypes = ["void", "uint8_t", "uint16_t", "uint32_t", "int8_t",
                 "int16_t", "int32_t", "char", "string", "unsigned char", "int",
                 "unsigned int", "short", "unsigned short", "long", "unsigned long",
                 "long long", "float", "double", "size_t", "UNITY_LINE_TYPE"]
    macros = ['#if', '#endif', '{', '}']
    output = []

    for item in usedDataTypes:
        for lineFunParams in item.insideStruct:
            if (len(lineFunParams[0]) - 1 >= 0):
                if (lineFunParams[0][len(lineFunParams[0]) - 1] == '*'):
                    # it's probably pointer
                    lineFunParams[0] = lineFunParams[0][:-1]
                known = False

                index = 0
                if (lineFunParams[0] == '__IO' or lineFunParams[0] == 'volatile'):
                    lineFunParams[0] = 'volatile'
                    index = 1

                for s in dataTypes:
                    if (s == lineFunParams[index]):
                        known = True
                        break
                for y in notKnownDataTypes:
                    if (y.dataName == lineFunParams[index]):
                        known = True
                        break
                # check if already doesn't exist
                for x in output:
                    if (x == lineFunParams[index]):
                        known = True
                        break

                # additional filtering (proprocessor and } sign)
                for macro in macros:
                    if (macro == lineFunParams[index]):
                        known = True
                        break
                
                if (known == False):
                    #print("Appending: " + lineFunParams[index])
                    output.append(DataType(lineFunParams[index], 0, 0, 'N/A', item.predecesor, 'N/A'))
            #print(notKnownDataTypes)
    
    return output

def getInsideOfStruct(dataType):
    file1 = open(dataType.filename, 'r')
    tmpContent = file1.readlines()
    obj = DataType('', 0, 0, 'None', 'None', 'None')
    for line_number in range(dataType.startLine, dataType.endLine - 1):
        if not(tmpContent[line_number].startswith(" " * 6)):
            obj.insideStruct.append((" ".join(tmpContent[line_number].split())).split(" "))
        # existingInStruct.append(tmpContent[line_number])
    file1.close()
    obj.predecesor = dataType.dataName
    #print('Inside Struct predecesor ---XXXXXX : ' + obj.predecesor)

    return obj

def CheckIfAlreadyExistDataType(newNotKnown, dataTypesList):
    for x in dataTypesList:
        if (x == newNotKnown):
            return True
    return False

def CheckIfExistInFoundDataTypes(elem, found):
    for x in found:
        if (x.dataName == elem.dataName):
            return True
    return False

def LookForFunctionPointers(fname, existingNotKnownDataTypes):
    listOfNotKnownDataTypes = []
    #print(fname)

    fileHALDriver = open(fname, 'r')
    contentHALDriver = fileHALDriver.readlines()

    lineCounter = 1
    encounteredLine = 1
    for line in contentHALDriver:
        for x in existingNotKnownDataTypes:
            if re.match('typedef\s*(\w+)\s*\(.*' + rf'{re.escape(x.dataName)}' + '\s*\)', line):
                listOfNotKnownDataTypes.append(DataType(x.dataName, lineCounter, lineCounter, 'FunPtr', x.dataName, fname))
        lineCounter += 1

    fileHALDriver.close()
    return listOfNotKnownDataTypes

def SortDataTypes(found):
    sortedList = []
    # Add enums, because the order of them is meaningless
    for item in found:
        if (item.type == 'enum'):
            sortedList.append(item)
    
    # Nextly there must be sorted structs
    # First level (on last place) is the struct which has
    # same name as predecesor (it is struct used in mock)
    structLevelSortedIncreased = []
    for item in found:
        if (item.dataName == item.predecesor and item.type == 'struct'):
            item.level = 1
            structLevelSortedIncreased.append(item)
    # assign levels for all struct data types (do it till all struct data types will have level different than 0)
    isAllZero = True
    while (isAllZero):
        for item in found:
            for item2 in structLevelSortedIncreased:
                if (item.predecesor == item2.dataName and item.dataName != item.predecesor and item.type == 'struct'):
                    item.level = item2.level + 1
                    structLevelSortedIncreased.append(item)

        isAllZero = False
        for item in found:
            if (item.type =='struct' and item.level == 0):
                isAllZero = True

    for x in structLevelSortedIncreased:
        print(x.dataName + ' ------ ' + str(x.level))

    structLevelSortedDecreased = sorted(structLevelSortedIncreased, key=lambda DataType: DataType.level, reverse=True)

    for x in structLevelSortedDecreased:
        print(x.dataName + ' ------ ' + str(x.level))

    sortedList.extend(structLevelSortedDecreased)
    
    return sortedList

def main():
    n = len(sys.argv)

    if (n != 3):
        print("Incorrect number of parameters")
        exit(0)
    print("hello")

    content = []
    fn = sys.argv[1]
    headerPath = sys.argv[2]
    if os.path.exists(fn):
        print(os.path.basename(fn))
        file = open(fn, 'r')
        content = file.readlines()
        usedDataTypes = SearchFunctionDeclarations(content)

        notKnownDataTypes = []
        notKnownDataTypes = GetNotKnownDataTypes(usedDataTypes)
        print('############### notKnownDataTypes ##########################')
        for x in notKnownDataTypes:
            print(x.dataName)

        # ABOVE - reading the mock file content and getting data types, then getting uniq data types (user specified)
        # NEXT - searching in directories for those data types
        foundNotKnownDataTypes = []



        for entry in os.scandir("/home/marzd/Documents/Projects/stm32/AudioSpectrum/HalVersion/Drivers/STM32F4xx_HAL_Driver/Inc/"):
            if (entry.path.endswith(".h")):
                tmp = LookForNotKnownDataTypes(entry.path, notKnownDataTypes)
                tmp2 = LookForFunctionPointers(entry.path, notKnownDataTypes)
                if (len(tmp) != 0):
                    foundNotKnownDataTypes.extend(tmp)
                if (len(tmp2) != 0):
                    foundNotKnownDataTypes.extend(tmp2)

        for x in foundNotKnownDataTypes:
            print(x.dataName + " --- " + x.filename + " --- " + str(x.startLine) + " - " + str(x.endLine) + " --- " + x.type + " --- " + x.predecesor)

        # # get data types which are inside struct
        existingInStruct = []
        for x in foundNotKnownDataTypes:
            if (x.type == 'struct'):
                existingInStruct.append(getInsideOfStruct(x))

        notKnownTmp = GetNotKnownDataTypesFromStruct(existingInStruct, notKnownDataTypes)
        notKnownDataTypes.extend(notKnownTmp)

        print('######## Not known inside struct  ###########')
        for x in notKnownDataTypes:
            print(x.dataName + ' --------- ' + x.predecesor)

        for i in range(0,3):
            existingInStruct.clear()
            for entry in os.scandir("/home/marzd/Documents/Projects/stm32/AudioSpectrum/HalVersion/Drivers/STM32F4xx_HAL_Driver/Inc/"):
                if (entry.path.endswith(".h")):
                    tmp = LookForNotKnownDataTypes(entry.path, notKnownDataTypes)
                    if (len(tmp) != 0):
                        for elem in tmp:
                            if not(CheckIfExistInFoundDataTypes(elem, foundNotKnownDataTypes)):
                                foundNotKnownDataTypes.append(elem)
            for entry in os.scandir("/home/marzd/Documents/Projects/stm32/AudioSpectrum/HalVersion/Drivers/CMSIS/Device/ST/STM32F4xx/Include"):
                if (entry.path.endswith(".h")):
                    tmp = LookForNotKnownDataTypes(entry.path, notKnownDataTypes)
                    if (len(tmp) != 0):
                        for elem in tmp:
                            if not(CheckIfExistInFoundDataTypes(elem, foundNotKnownDataTypes)):
                                foundNotKnownDataTypes.append(elem)

            for x in foundNotKnownDataTypes:
                if (x.type == 'struct'):
                    existingInStruct.append(getInsideOfStruct(x))
            notKnownTmp = GetNotKnownDataTypesFromStruct(existingInStruct, notKnownDataTypes)
            # extend only new not known data types
            for elem in notKnownTmp:
                if not(CheckIfAlreadyExistDataType(elem, notKnownDataTypes)):
                    notKnownDataTypes.append(elem)

            print('################### found Data Types for loop ' + str(i))
            for x in foundNotKnownDataTypes:
                print(x.dataName + " --- " + x.filename + " --- " + str(x.startLine) + " - " + str(x.endLine) + " --- " + x.type + " --- " + x.predecesor)
            print('################### Not Known Data Types (for loop) ' + str(i))
            for x in notKnownDataTypes:
                print(x.dataName)
                    
        print('############# foundNotKnownDataTypes ####################')
        for x in foundNotKnownDataTypes:
            print(x.dataName + " --- " + x.filename + " --- " + str(x.startLine) + " - " + str(x.endLine) + " --- " + x.type + " --- " + x.predecesor)

        file.close()
        

        # We have data types in foundNotKnownDataTypes
        # Now it's time to sort things and create file
        outputData = SortDataTypes(foundNotKnownDataTypes)
        for x in foundNotKnownDataTypes:
            if (x.type == "FunPtr"):
                outputData.append(x)

        headerFile = os.path.split(headerPath)
        # Write to new file
        f = open(headerPath, "w")
        f.write("/*******GENERATED CONTENT******/\n")
        for data in outputData:
            fileOp = open(data.filename, 'r')
            contentFileOp = fileOp.readlines()
            for lineNum in range(data.startLine, data.endLine + 1):
                contentFileOp[lineNum - 1] = contentFileOp[lineNum - 1].replace("__IO", "volatile")
                f.write(contentFileOp[lineNum - 1])

            fileOp.close()
            f.write('\n')
        f.close()


        fSingleLine = open(fn, 'r')    # pass an appropriate path of the required file
        lines = fSingleLine.readlines()
        lines[6 - 1] = "// " + lines[6 - 1]    # n is the line number you want to edit; subtract 1 as indexing of list starts from 0
        lines[7 - 1] = "#include \"" + headerFile[1] + "\"\n"
        lines.insert(7, '\n')

        fSingleLine.close()   # close the file and reopen in write mode to enable writing to file; you can also open in append mode and use "seek", but you will have some unwanted old data if the new data is shorter in length.

        fSingleLine = open(fn, 'w')
        fSingleLine.writelines(lines)
        # do the remaining operations on the file
        fSingleLine.close()
        



if __name__ == "__main__":
    main()

