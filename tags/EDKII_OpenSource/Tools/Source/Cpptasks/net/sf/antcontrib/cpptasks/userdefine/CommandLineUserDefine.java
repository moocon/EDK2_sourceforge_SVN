/*
 * 
 * Copyright 2001-2004 The Ant-Contrib project
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */
package net.sf.antcontrib.cpptasks.userdefine;

import java.io.File;
import java.util.Iterator;
import java.util.LinkedHashSet;
import java.util.Set;
import java.util.StringTokenizer;
import java.util.Vector;

import net.sf.antcontrib.cpptasks.CCTask;
import net.sf.antcontrib.cpptasks.CUtil;
import net.sf.antcontrib.cpptasks.types.CommandLineArgument;
import net.sf.antcontrib.cpptasks.types.ConditionalFileSet;

import org.apache.tools.ant.BuildException;
import org.apache.tools.ant.DirectoryScanner;
import org.apache.tools.ant.Project;

public class CommandLineUserDefine {

    String includePathDelimiter = null;
    
    String outputDelimiter = null;
    
    public void command(CCTask cctask, UserDefineDef userdefine){
        boolean isGccCommand = userdefine.getFamily().equalsIgnoreCase("GCC");
        File workdir;
        Project project = cctask.getProject();
        if(userdefine.getWorkdir() == null) {
            workdir = new File(".");
        }
        else {
            workdir = userdefine.getWorkdir();
        } 
        
        //
        // generate cmdline= command + args + includepath + endargs + outfile
        // 
        Vector args = new Vector();
        Vector argsWithoutSpace = new Vector();
        Vector endargs = new Vector();
        Vector endargsWithoutSpace = new Vector();
        Vector includePath = new Vector();
        
        //
        // Generate cmdline = command        + 
        //                    general args   + 
        //                    outputflag     + outputfile
        //                    includpath            + 
        //                    endargs                          + 
        // 
        
        //
        // get Args.
        //
        CommandLineArgument[] argument = userdefine.getActiveProcessorArgs();
        for (int j = 0; j < argument.length; j++) {
            if (argument[j].getLocation() == 0) {
                args.addElement(argument[j].getValue());
            } else {
                endargs.addElement(argument[j].getValue());
            }
        }
        
        //
        // get include path.
        //
        String[] incPath = userdefine.getActiveIncludePaths();
        for (int j = 0; j < incPath.length; j++) {
            if(incPath[j].indexOf(' ') >= 0) {
                includePath.addElement( includePathDelimiter + incPath[j]);
                //includePath.addElement( includePathDelimiter + "\"" + incPath[j] + "\"");
            }
            else {
                includePath.addElement( includePathDelimiter + incPath[j]);
            }
        }
        
        //
        // Remove space in args and endargs.
        //
        for ( int i=0; i < args.size(); i++) {
            String str = (String)args.get(i);
            StringTokenizer st = new StringTokenizer(str, " \t");
            while(st.hasMoreTokens()) {
                argsWithoutSpace.addElement(st.nextToken());
            }
        }
        for ( int i=0; i < endargs.size(); i++) {
            String str = (String)endargs.get(i);
            StringTokenizer st = new StringTokenizer(str, " \t");
            while(st.hasMoreTokens()) {
                endargsWithoutSpace.addElement(st.nextToken());
            }
        }
        
        int cmdLen = 0;
        //
        // command + args + endargs + includepath + sourcefile
        //
        cmdLen = 1 + argsWithoutSpace.size() + endargsWithoutSpace.size() + includePath.size() + 1;
        String[] libSet = userdefine.get_libset();
        if (libSet != null && libSet.length > 0){
            cmdLen = cmdLen + libSet.length;
            if (isGccCommand) {
                cmdLen += 2;    // we need -( and -) to group libs for GCC
            }
        }

        //
        // In gcc the "cr" flag should follow space then add outputfile name, otherwise
        // it will pop error. 
        // TBD
        if (outputDelimiter != null && userdefine.getOutputFile() != null && outputDelimiter.trim().length() > 0){
            if (outputDelimiter.trim().equalsIgnoreCase("-cr")){
                cmdLen = cmdLen + 2;
            }else {
                cmdLen++;
            }
        }
        
        //
        //  for every source file
        //  if file is header file, just skip it (add later)
        //
        Vector srcSets = userdefine.getSrcSets();
//        System.out.println("##" + userdefine.getSrcSets());
        
        //
        // if have source file append source file in command land.
        //
        Set allSrcFiles = new LinkedHashSet();
        for (int i = 0; i < srcSets.size(); i++) {
            ConditionalFileSet srcSet = (ConditionalFileSet) srcSets
                    .elementAt(i);
            if (srcSet.isActive()) {
                // Find matching source files
                DirectoryScanner scanner = srcSet.getDirectoryScanner(project);
                // Check each source file - see if it needs compilation
                String[] fileNames = scanner.getIncludedFiles();
                for (int j = 0; j < fileNames.length; j++){
                    // execute the command
                    allSrcFiles.add(scanner.getBasedir() + "/" + fileNames[j]);
                    if (isGccCommand) {
                        System.out.println("[" + userdefine.getType() + "] " + fileNames[j]);
                    }
                }
            }
        }

        String[] fileNames = (String[])allSrcFiles.toArray(new String[allSrcFiles.size()]);
        String[] cmd = new String[cmdLen - 1 + fileNames.length];
        int index = 0;
        cmd[index++] = userdefine.getCmd();
        
        Iterator iter = argsWithoutSpace.iterator();
        while (iter.hasNext()) {
            cmd[index++] = project.replaceProperties((String)iter.next());
        }
        
        iter = endargsWithoutSpace.iterator();
        while (iter.hasNext()) {
            cmd[index++] = project.replaceProperties((String)iter.next());
        }
        
        //
        // Add outputFileFlag and output file to cmd
        //
        if (outputDelimiter != null && userdefine.getOutputFile() != null && outputDelimiter.length()> 0){
            if (outputDelimiter.trim().equalsIgnoreCase("-cr")){
                cmd[index++] = outputDelimiter;
                cmd[index++] = userdefine.getOutputFile();
            }else {
                cmd[index++] =  outputDelimiter + userdefine.getOutputFile();
            }
        }
        
        iter = includePath.iterator();
        while (iter.hasNext()) {
            cmd[index++] = (String)iter.next();
        }
        
        if (libSet != null && libSet.length > 0){
            if (isGccCommand) {
                cmd[index++] = "-(";
            }
            for (int k = 0; k < libSet.length ; k++){
                cmd[index++] = libSet[k];
            }
            if (isGccCommand) {
                cmd[index++] = "-)";
            }
        }
        for (int j = 0; j < fileNames.length; j++){
            // execute the command
            cmd[index++] = fileNames[j];
        }
        
        int retval = runCommand(cctask, workdir, cmd);
        // if with monitor, add more code
        if (retval != 0) {
           throw new BuildException(userdefine.getCmd()
                   + " failed with return code " + retval,
                   cctask.getLocation());
        }
    }
    
    protected int runCommand(CCTask task, File workingDir, String[] cmdline)
                    throws BuildException {
        return CUtil.runCommand(task, workingDir, cmdline, false, null);

    }

    protected String getInputFileArgument(File outputDir, String filename,
                    int index) {
        //
        // if there is an embedded space,
        // must enclose in quotes
        if (filename.indexOf(' ') >= 0) {
            StringBuffer buf = new StringBuffer("\"");
            buf.append(filename);
            buf.append("\"");
            return buf.toString();
        }
        return filename;
    }

}