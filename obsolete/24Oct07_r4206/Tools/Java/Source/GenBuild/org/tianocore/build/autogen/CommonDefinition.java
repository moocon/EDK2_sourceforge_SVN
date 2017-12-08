/** @file
 CommonDefinition class.

 This class is to define some common marcos and funcions, which used by AutoGen.
 
 Copyright (c) 2006, Intel Corporation
 All rights reserved. This program and the accompanying materials
 are licensed and made available under the terms and conditions of the BSD License
 which accompanies this distribution.  The full text of the license may be found at
 http://opensource.org/licenses/bsd-license.php
 
 THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
 WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

 **/
package org.tianocore.build.autogen;

import java.util.Iterator;
import java.util.LinkedHashSet;
import java.util.Set;
import org.tianocore.common.definitions.EdkDefinitions;
import org.tianocore.common.definitions.ToolDefinitions;
/**
  CommonDefinition
  
  This class is to define some common marcos, which used by AutoGen.
  
**/
public class CommonDefinition {

	///
	///  final static string
	/// 
    public final static String LIBRARY            = "LIBRARY";

    public final static String AUTOGENHBEGIN      = "extern int __make_me_compile_correctly;"
											      + ToolDefinitions.LINE_SEPARATOR;

    public final static String INCLUDE            = "#include";

    //public final static String DEBUGPROPERYMASK   = "const UINT8  _gDebugPropertyMask "
    //                                              + "= DEBUG_PROPERTY_DEBUG_ASSERT_ENABLED"
    //                                              + "  | DEBUG_PROPERTY_DEBUG_PRINT_ENABLED"
    //                                              + "  | DEBUG_PROPERTY_DEBUG_CODE_ENABLED;" 
    //		                                          + ToolDefinitions.LINE_SEPARATOR;

    //public final static String DEFAULERROLEVEL    = "const UINTN  _gModuleDefaultErrorLevel"
    //                                              + " = EFI_D_ERROR | EFI_D_LOAD;"
	//										      + ToolDefinitions.LINE_SEPARATOR;


    public final static String INCLUDEAUTOGENH    = INCLUDE 
		                                          + "    <AutoGen.h>"
												  + ToolDefinitions.LINE_SEPARATOR;

    public final static String DEFINE             = "#define         ";

    public final static String GEFI               = "gEfi";

    public final static String PRTOCOLGUID        = "ProtocolGuid";

    public final static String PPIGUID            = "PpiGuid";

    public final static String GUID               = "Guid";

    public final static String TIANOR8PLASHMAPH   = "TianoR8FlashMap.h";

    public final static String FLASHMAPH          = "FlashMap.h";

	public final static String IFNDEF             = "#ifndef          ";

	public final static String AUTOGENH            = "_AUTOGENH_";

                                                                 
    ///
    /// AutoGen.h and AutoGen.c file's header
    ///
    public final static String AUTOGENHNOTATION   = "/**"
	                                              + ToolDefinitions.LINE_SEPARATOR
	                                              + "  DO NOT EDIT"
                                                  + ToolDefinitions.LINE_SEPARATOR
	                                              + "  FILE auto-generated by GenBuild tasks"
	                                              + ToolDefinitions.LINE_SEPARATOR 
	                                              + "  Module name:"
	                                              + ToolDefinitions.LINE_SEPARATOR
	                                              + "       AutoGen.h"
	                                              + ToolDefinitions.LINE_SEPARATOR 
	                                              + "  Abstract:" 
	                                              + "       Auto-generated AutoGen.h for building module or library."
	                                              + ToolDefinitions.LINE_SEPARATOR
                                                  + "**/"
                                                  + ToolDefinitions.LINE_SEPARATOR 
                                                  + ToolDefinitions.LINE_SEPARATOR;

    public final static String AUTOGENCNOTATION   = "/**"
		                                          + ToolDefinitions.LINE_SEPARATOR
		                                          + "  DO NOT EDIT"
												  + ToolDefinitions.LINE_SEPARATOR
		                                          + "  FILE auto-generated by GenBuild tasks"
												  + ToolDefinitions.LINE_SEPARATOR 
		                                          + "  Module name:"
		                                          + ToolDefinitions.LINE_SEPARATOR 
		                                          + "       AutoGen.c"
		                                          + ToolDefinitions.LINE_SEPARATOR 
		                                          + "  Abstract:" 
		                                          + "       Auto-generated AutoGen.c for building module or library."
												  + ToolDefinitions.LINE_SEPARATOR 
		                                          + "**/"
		                                          + ToolDefinitions.LINE_SEPARATOR
		                                          + ToolDefinitions.LINE_SEPARATOR;

    ///
    /// The defintions for identifying current module
    /// is PEI Pcd driver or Dxe Pcd driver.
    /// 
    public static enum   PCD_DRIVER_TYPE { NOT_PCD_DRIVER,
                                           PEI_PCD_DRIVER, 
                                           DXE_PCD_DRIVER,
                                           UNKNOWN_PCD_DRIVER
		                                 };


    ///
    /// module type
    ///
    public final static int ModuleTypeBase             = 0;
    public final static int ModuleTypeSec              = 1;
    public final static int ModuleTypePeiCore          = 2;
    public final static int ModuleTypePeim             = 3;
    public final static int ModuleTypeDxeCore          = 4;
    public final static int ModuleTypeDxeDriver        = 5;
    public final static int ModuleTypeDxeRuntimeDriver = 6;
    public final static int ModuleTypeDxeSmmDriver     = 7;
    public final static int ModuleTypeDxeSalDriver     = 8;
    public final static int ModuleTypeUefiDriver       = 9;
    public final static int ModuleTypeUefiApplication  = 10;
    public final static int ModuleTypeUnknown          = 11;

    ///
    /// Usaged style
    ///
    public final static String ALWAYSCONSUMED = "ALWAYS_CONSUMED";
    public final static String ALWAYSPRODUCED = "ALWAYS_PRODUCED";


    public static class MyEnum {
        String moduleTypeStr;
        int   type;

        MyEnum (String str, int type) {
            this.type    = type;
            this.moduleTypeStr = str;
        }

        int ForInt(String str) {
            if (str.equals(this.moduleTypeStr)) {
                return this.type;
            } else
                return -1;
        }
    }

    ///
    /// Module type
    ///
    public static final MyEnum[] moduleEnum = new MyEnum[] {
        new MyEnum(EdkDefinitions.MODULE_TYPE_BASE, ModuleTypeBase),
        new MyEnum(EdkDefinitions.MODULE_TYPE_SEC, ModuleTypeSec),
        new MyEnum(EdkDefinitions.MODULE_TYPE_PEI_CORE, ModuleTypePeiCore),
        new MyEnum(EdkDefinitions.MODULE_TYPE_PEIM, ModuleTypePeim),
        new MyEnum(EdkDefinitions.MODULE_TYPE_DXE_CORE, ModuleTypeDxeCore),
        new MyEnum(EdkDefinitions.MODULE_TYPE_DXE_DRIVER, ModuleTypeDxeDriver),
        new MyEnum(EdkDefinitions.MODULE_TYPE_DXE_RUNTIME_DRIVER, ModuleTypeDxeRuntimeDriver),
        new MyEnum(EdkDefinitions.MODULE_TYPE_DXE_SAL_DRIVER, ModuleTypeDxeSalDriver),
        new MyEnum(EdkDefinitions.MODULE_TYPE_DXE_SMM_DRIVER, ModuleTypeDxeSmmDriver),
        new MyEnum(EdkDefinitions.MODULE_TYPE_UEFI_DRIVER, ModuleTypeUefiDriver),
        new MyEnum(EdkDefinitions.MODULE_TYPE_UEFI_APPLICATION, ModuleTypeUefiApplication)};

    /**
      getModuleType
      
      This function get the module type value according module type string.
      
      @param  moduleTypeStr     String of modlue type.
      @return                   
    **/
    public static int getModuleType(String moduleTypeStr) {
        int returnValue = -1;
        for (int i = 0; i < CommonDefinition.moduleEnum.length; i++) {
            returnValue = CommonDefinition.moduleEnum[i].ForInt(moduleTypeStr);
            if (returnValue != -1) {
                return returnValue;
            }
        }
        return CommonDefinition.ModuleTypeUnknown;
    }

    /**
      formateGuidName
      
      This function is to formate GUID to ANSI c form.
      
      @param guidNameCon
                 String of GUID.
      @return Formated GUID.
    **/
    public static String formatGuidName(String guidNameConv) {
        String[] strList;
        String guid = "";
        int index = 0;
        if (guidNameConv
            .matches("[a-fA-F0-9]{8}-[a-fA-F0-9]{4}-[a-fA-F0-9]{4}-[a-fA-F0-9]{4}-[a-fA-F0-9]{12}")) {
            strList = guidNameConv.split("-");
            guid = "0x" + strList[0] + ", ";
            guid = guid + "0x" + strList[1] + ", ";
            guid = guid + "0x" + strList[2] + ", ";
            guid = guid + "{";
            guid = guid + "0x" + strList[3].substring(0, 2) + ", ";
            guid = guid + "0x" + strList[3].substring(2, 4);

            while (index < strList[4].length()) {
                guid = guid + ", ";
                guid = guid + "0x" + strList[4].substring(index, index + 2);
                index = index + 2;
            }
            guid = guid + "}";
            return guid;
        } else if (guidNameConv
                   .matches("0x[a-fA-F0-9]{1,8},( )*0x[a-fA-F0-9]{1,4},( )*0x[a-fA-F0-9]{1,4}(,( )*\\{)?(,?( )*0x[a-fA-F0-9]{1,2}){8}( )*(\\})?")) {
            strList = guidNameConv.split(",");

            //
            // chang Microsoft specific form to ANSI c form
            //
            for (int i = 0; i < 3; i++) {
                guid = guid + strList[i] + ",";
            }
            guid = guid + "{";

            for (int i = 3; i < strList.length; i++) {
                if (i == strList.length - 1) {
                    guid = guid + strList[i];
                } else {
                    guid = guid + strList[i] + ",";
                }
            }
            guid = guid + "}";
            return guid;
        } else {
            System.out
            .println("Check GUID Value, It doesn't conform to the registry format specified in the schema!!!");
            return "0";

        }
    }

    /**
      Remove deuplicat string in list
      
      This function is to duplicat string in list
      
      @param String[]
                 String list.
      @return String[] String list which remove the duplicate string.
    **/
    public static String[] remDupString (String[] orgList){
        Set<String> strList = new LinkedHashSet<String>();
        String[] desList ;
        if (orgList == null) {
            return new String[0];
        }
        for (int i = 0; i < orgList.length; i++) {
            strList.add(orgList[i]);
        }
        desList = new String[strList.size()];
        Iterator item = strList.iterator();
        int index = 0;
        while (item.hasNext()) {
            desList[index] = (String)item.next();
            index++;
        }
        return desList;
    }

}