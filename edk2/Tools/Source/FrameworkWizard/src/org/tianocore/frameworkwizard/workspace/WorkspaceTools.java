/** @file

 The file is used to init workspace and get basic information of workspace
 
 Copyright (c) 2006, Intel Corporation
 All rights reserved. This program and the accompanying materials
 are licensed and made available under the terms and conditions of the BSD License
 which accompanies this distribution.  The full text of the license may be found at
 http://opensource.org/licenses/bsd-license.php
 
 THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
 WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
 
 **/
package org.tianocore.frameworkwizard.workspace;

import java.io.File;
import java.io.IOException;
import java.util.List;
import java.util.Vector;

import org.apache.xmlbeans.XmlCursor;
import org.apache.xmlbeans.XmlException;
import org.tianocore.DbPathAndFilename;
import org.tianocore.FrameworkDatabaseDocument.FrameworkDatabase;
import org.tianocore.IndustryStdIncludesDocument.IndustryStdIncludes;
import org.tianocore.ModuleSurfaceAreaDocument.ModuleSurfaceArea;
import org.tianocore.MsaFilesDocument.MsaFiles;
import org.tianocore.MsaHeaderDocument.MsaHeader;
import org.tianocore.PackageSurfaceAreaDocument.PackageSurfaceArea;
import org.tianocore.PlatformHeaderDocument.PlatformHeader;
import org.tianocore.PlatformSurfaceAreaDocument.PlatformSurfaceArea;
import org.tianocore.SourceFilesDocument.SourceFiles;
import org.tianocore.SpdHeaderDocument.SpdHeader;
import org.tianocore.frameworkwizard.common.DataType;
import org.tianocore.frameworkwizard.common.Log;
import org.tianocore.frameworkwizard.common.OpenFile;
import org.tianocore.frameworkwizard.common.SaveFile;
import org.tianocore.frameworkwizard.common.Sort;
import org.tianocore.frameworkwizard.common.Tools;
import org.tianocore.frameworkwizard.common.Identifications.Identification;
import org.tianocore.frameworkwizard.far.FarHeader;
import org.tianocore.frameworkwizard.far.FarIdentification;
import org.tianocore.frameworkwizard.module.Identifications.ModuleIdentification;
import org.tianocore.frameworkwizard.module.Identifications.PcdCoded.PcdIdentification;
import org.tianocore.frameworkwizard.module.Identifications.PcdCoded.PcdVector;
import org.tianocore.frameworkwizard.packaging.PackageIdentification;
import org.tianocore.frameworkwizard.platform.PlatformIdentification;

public class WorkspaceTools {
    //
    // Define class members
    //
    private FrameworkDatabase fdb = null;

    private Vector<ModuleIdentification> vModuleList = new Vector<ModuleIdentification>();

    private Vector<PackageIdentification> vPackageList = new Vector<PackageIdentification>();

    private Vector<PlatformIdentification> vPlatformList = new Vector<PlatformIdentification>();

    /**
     
     Open Framework Database file
     
     */
    private FrameworkDatabase openFrameworkDb() {
        String strFrameworkDbFilePath = Workspace.getCurrentWorkspace() + Workspace.getStrWorkspaceDatabaseFile();
        strFrameworkDbFilePath = Tools.convertPathToCurrentOsType(strFrameworkDbFilePath);
        try {
            fdb = OpenFile.openFrameworkDb(strFrameworkDbFilePath);
        } catch (XmlException e) {
            Log.err("Open Framework Database " + strFrameworkDbFilePath, e.getMessage());
            return null;
        } catch (Exception e) {
            Log.err("Open Framework Database " + strFrameworkDbFilePath, "Invalid file type");
            return null;
        }
        return fdb;
    }

    public void addFarToDb(List<String> packageList, List<String> platformList, FarHeader far) {
        FrameworkDatabase fdb = openFrameworkDb();

        for (int i = 0; i < packageList.size(); i++) {
            DbPathAndFilename item = DbPathAndFilename.Factory.newInstance();
            item.setFarGuid(far.getGuidValue());
            item.setStringValue(packageList.get(i));
            fdb.getPackageList().getFilenameList().add(item);
        }

        for (int i = 0; i < platformList.size(); i++) {
            DbPathAndFilename item = DbPathAndFilename.Factory.newInstance();
            item.setFarGuid(far.getGuidValue());
            item.setStringValue(platformList.get(i));
            fdb.getPlatformList().getFilenameList().add(item);
        }

        DbPathAndFilename farItem = DbPathAndFilename.Factory.newInstance();
        farItem.setFarGuid(far.getGuidValue());
        farItem.setStringValue(far.getFarName());
        fdb.getFarList().getFilenameList().add(farItem);

        String strFrameworkDbFilePath = Workspace.getCurrentWorkspace() + Workspace.getStrWorkspaceDatabaseFile();
        strFrameworkDbFilePath = Tools.convertPathToCurrentOsType(strFrameworkDbFilePath);

        try {
            SaveFile.saveDbFile(strFrameworkDbFilePath, fdb);
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    public void removeFarFromDb(FarIdentification far) {
        FrameworkDatabase fdb = openFrameworkDb();
        //
        // Remove Packages
        //
        XmlCursor cursor = fdb.getPackageList().newCursor();
        cursor.toFirstChild();
        do {
            DbPathAndFilename item = (DbPathAndFilename) cursor.getObject();

            if (item.getFarGuid() != null && item.getFarGuid().equalsIgnoreCase(far.getGuid())) {
                cursor.removeXml();
            }
        } while (cursor.toNextSibling());
        cursor.dispose();

        //
        // Remove Platforms
        //
        cursor = fdb.getPlatformList().newCursor();
        cursor.toFirstChild();
        do {
            DbPathAndFilename item = (DbPathAndFilename) cursor.getObject();
            if (item.getFarGuid() != null && item.getFarGuid().equalsIgnoreCase(far.getGuid())) {
                cursor.removeXml();
            }
        } while (cursor.toNextSibling());

        //
        // Remove Far
        //
        cursor = fdb.getFarList().newCursor();
        cursor.toFirstChild();
        do {
            DbPathAndFilename item = (DbPathAndFilename) cursor.getObject();
            if (item.getFarGuid() != null && item.getFarGuid().equalsIgnoreCase(far.getGuid())) {
                cursor.removeXml();
            }
        } while (cursor.toNextSibling());
        cursor.dispose();

        String strFrameworkDbFilePath = Workspace.getCurrentWorkspace() + Workspace.getStrWorkspaceDatabaseFile();
        strFrameworkDbFilePath = Tools.convertPathToCurrentOsType(strFrameworkDbFilePath);
        try {
            SaveFile.saveDbFile(strFrameworkDbFilePath, fdb);
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    public String getPackageFarGuid(Identification packageId) {
        openFrameworkDb();

        for (int index = 0; index < fdb.getPackageList().getFilenameList().size(); index++) {
            DbPathAndFilename item = fdb.getPackageList().getFilenameArray(index);
            String path = Workspace.getCurrentWorkspace() + DataType.FILE_SEPARATOR + item.getStringValue();
            File tempFile = new File(path);
            if (tempFile.getPath().equalsIgnoreCase(packageId.getPath())) {
                return fdb.getPackageList().getFilenameArray(index).getFarGuid();
            }
        }

        return null;
    }

    public String getPlatformFarGuid(Identification platformId) {
        openFrameworkDb();

        for (int index = 0; index < fdb.getPlatformList().getFilenameList().size(); index++) {
            DbPathAndFilename item = fdb.getPlatformList().getFilenameArray(index);
            String path = Workspace.getCurrentWorkspace() + DataType.FILE_SEPARATOR + item.getStringValue();
            File tempFile = new File(path);
            if (tempFile.getPath().equalsIgnoreCase(platformId.getPath())) {
                return fdb.getPlatformList().getFilenameArray(index).getFarGuid();
            }
        }

        return null;
    }

    public String getModuleFarGuid(Identification moduleId) {
        PackageIdentification packageId = getPackageIdByModuleId(moduleId);
        if (packageId != null) {
            return getPackageFarGuid(packageId);
        } else {
            return null;
        }
    }

    /**
     Get all modules' paths from one package
     
     @return a Vector with all modules' path
     
     **/
    public Vector<String> getAllModulesOfPackage(String path) {
        Vector<String> modulePath = new Vector<String>();
        try {
            MsaFiles files = OpenFile.openSpdFile(path).getMsaFiles();
            if (files != null) {
                for (int index = 0; index < files.getFilenameList().size(); index++) {
                    String msaPath = files.getFilenameList().get(index);
                    msaPath = Tools.addFileSeparator(Tools.getFilePathOnly(path)) + msaPath;
                    msaPath = Tools.convertPathToCurrentOsType(msaPath);
                    modulePath.addElement(msaPath);
                }
            }
        } catch (IOException e) {

        } catch (XmlException e) {

        } catch (Exception e) {

        }
        return modulePath;
    }

    /**
     Get all Industry Std Includes' paths from one package
     
     @return a Vector with all paths
     
     **/
    public Vector<String> getAllIndustryStdIncludesOfPackage(String path) {
        Vector<String> includePath = new Vector<String>();
        try {
            IndustryStdIncludes files = OpenFile.openSpdFile(path).getIndustryStdIncludes();
            if (files != null) {
                for (int index = 0; index < files.getIndustryStdHeaderList().size(); index++) {
                    String temp = files.getIndustryStdHeaderList().get(index).getIncludeHeader();
                    temp = Tools.addFileSeparator(Tools.getFilePathOnly(path)) + temp;
                    temp = Tools.convertPathToCurrentOsType(temp);
                    includePath.addElement(temp);
                }
            }
        } catch (IOException e) {

        } catch (XmlException e) {

        } catch (Exception e) {

        }
        return includePath;
    }

    /**
     Get all package basic information form the FrameworkDatabase.db file
     
     @return vPackageList A vector includes all packages' basic information
     
     */
    public Vector<PackageIdentification> getAllPackages() {
        Identification id = null;
        vPackageList.removeAllElements();

        openFrameworkDb();

        for (int index = 0; index < fdb.getPackageList().getFilenameList().size(); index++) {
            String path = Workspace.getCurrentWorkspace() + DataType.FILE_SEPARATOR
                          + fdb.getPackageList().getFilenameArray(index).getStringValue();
            path = Tools.convertPathToCurrentOsType(path);
            try {
                id = getId(path, OpenFile.openSpdFile(path));
                vPackageList.addElement(new PackageIdentification(id));
            } catch (IOException e) {
                Log.err("Open Package Surface Area " + path, e.getMessage());

            } catch (XmlException e) {
                Log.err("Open Package Surface Area " + path, e.getMessage());

            } catch (Exception e) {
                Log.err("Open Package Surface Area " + path, "Invalid file type");

            }
        }
        Sort.sortPackages(vPackageList, DataType.SORT_TYPE_ASCENDING);
        return vPackageList;
    }

    public Vector<FarIdentification> getAllFars() {
        openFrameworkDb();
        Vector<FarIdentification> v = new Vector<FarIdentification>();
        for (int index = 0; index < fdb.getFarList().getFilenameList().size(); index++) {
            DbPathAndFilename item = fdb.getFarList().getFilenameList().get(index);
            FarIdentification far = new FarIdentification(item.getFarGuid(), item.getMd5Sum(), item.getStringValue());
            v.addElement(far);
        }
        return v;
    }

    public Vector<PackageIdentification> getPackagesByFar(FarIdentification far) {
        Identification id = null;
        openFrameworkDb();
        Vector<PackageIdentification> v = new Vector<PackageIdentification>();

        for (int index = 0; index < fdb.getPackageList().getFilenameList().size(); index++) {
            DbPathAndFilename item = fdb.getPackageList().getFilenameArray(index);
            String path = Workspace.getCurrentWorkspace() + DataType.FILE_SEPARATOR + item.getStringValue();
            path = Tools.convertPathToCurrentOsType(path);

            if (item.getFarGuid() != null && item.getFarGuid().equalsIgnoreCase(far.getGuid())) {

                try {
                    id = getId(path, OpenFile.openSpdFile(path));
                    v.addElement(new PackageIdentification(id));
                } catch (IOException e) {
                    Log.err("Open Package Surface Area " + path, e.getMessage());
                    e.printStackTrace();
                } catch (XmlException e) {
                    Log.err("Open Package Surface Area " + path, e.getMessage());
                    e.printStackTrace();
                } catch (Exception e) {
                    Log.err("Open Package Surface Area " + path, "Invalid file type");
                    e.printStackTrace();
                }
            }
        }
        return v;
    }

    public Vector<PlatformIdentification> getPlatformsByFar(FarIdentification far) {
        Identification id = null;
        openFrameworkDb();
        Vector<PlatformIdentification> v = new Vector<PlatformIdentification>();

        for (int index = 0; index < fdb.getPlatformList().getFilenameList().size(); index++) {
            DbPathAndFilename item = fdb.getPlatformList().getFilenameArray(index);
            String path = Workspace.getCurrentWorkspace() + DataType.FILE_SEPARATOR + item.getStringValue();
            path = Tools.convertPathToCurrentOsType(path);

            if (item.getFarGuid() != null && item.getFarGuid().equalsIgnoreCase(far.getGuid())) {
                try {
                    id = getId(path, OpenFile.openFpdFile(path));
                    v.addElement(new PlatformIdentification(id));
                } catch (IOException e) {
                    Log.err("Open Platform Surface Area " + path, e.getMessage());
                    e.printStackTrace();
                } catch (XmlException e) {
                    Log.err("Open Platform Surface Area " + path, e.getMessage());
                    e.printStackTrace();
                } catch (Exception e) {
                    Log.err("Open Platform Surface Area " + path, "Invalid file type");
                    e.printStackTrace();
                }
            }
        }
        return v;
    }

    /**
     Get all module basic information from a package
     
     @param id Package id
     @return A vector includes all modules' basic information
     
     **/
    public Vector<ModuleIdentification> getAllModules(PackageIdentification pid) {
        Vector<ModuleIdentification> v = new Vector<ModuleIdentification>();
        Vector<String> modulePaths = this.getAllModulesOfPackage(pid.getPath());
        Identification id = null;
        String modulePath = null;

        for (int index = 0; index < modulePaths.size(); index++) {
            try {
                modulePath = modulePaths.get(index);
                id = getId(modulePath, OpenFile.openMsaFile(modulePath));
            } catch (IOException e) {
                Log.log("Error when Open Module Surface Area " + modulePath, e.getMessage());
            } catch (XmlException e) {
                Log.log("Error when Open Module Surface Area " + modulePath, e.getMessage());
            } catch (Exception e) {
                Log.log("Error when Open Module Surface Area " + modulePath, "Invalid file type " + e.getMessage());
            }
            v.addElement(new ModuleIdentification(id, pid));
        }
        Sort.sortModules(v, DataType.SORT_TYPE_ASCENDING);
        return v;

    }

    /**
     Get all module basic information form the FrameworkDatabase.db file
     
     @return vModuleList A vector includes all modules' basic information
     
     */
    public Vector<ModuleIdentification> getAllModules() {
        vModuleList.removeAllElements();
        //
        // First get all packages
        //
        getAllPackages();
        Vector<String> modulePaths = new Vector<String>();
        Identification id = null;
        String packagePath = null;
        String modulePath = null;

        //
        // For each package, get all modules list
        //
        for (int indexI = 0; indexI < vPackageList.size(); indexI++) {
            packagePath = vPackageList.elementAt(indexI).getPath();
            modulePaths = this.getAllModulesOfPackage(packagePath);

            for (int indexJ = 0; indexJ < modulePaths.size(); indexJ++) {
                try {
                    modulePath = modulePaths.get(indexJ);
                    id = getId(modulePath, OpenFile.openMsaFile(modulePath));
                    vModuleList.addElement(new ModuleIdentification(id, vPackageList.elementAt(indexI)));
                } catch (IOException e) {
                    Log.err("Open Module Surface Area " + modulePath, e.getMessage());
                } catch (XmlException e) {
                    Log.err("Open Module Surface Area " + modulePath, e.getMessage());
                } catch (Exception e) {
                    Log.err("Open Module Surface Area " + modulePath, "Invalid file type");
                }
            }
        }
        Sort.sortModules(vModuleList, DataType.SORT_TYPE_ASCENDING);
        return vModuleList;
    }

    /**
     Get all platform basic information form the FrameworkDatabase.db file
     
     @return vplatformList A vector includes all platforms' basic information
     
     */
    public Vector<PlatformIdentification> getAllPlatforms() {
        Identification id = null;
        vPlatformList.removeAllElements();

        openFrameworkDb();

        for (int index = 0; index < fdb.getPlatformList().getFilenameList().size(); index++) {
            String path = Workspace.getCurrentWorkspace() + DataType.FILE_SEPARATOR
                          + fdb.getPlatformList().getFilenameArray(index).getStringValue();
            path = Tools.convertPathToCurrentOsType(path);
            try {
                id = getId(path, OpenFile.openFpdFile(path));
                vPlatformList.addElement(new PlatformIdentification(id));
            } catch (IOException e) {
                Log.err("Open Platform Surface Area " + path, e.getMessage());
            } catch (XmlException e) {
                Log.err("Open Platform Surface Area " + path, e.getMessage());
            } catch (Exception e) {
                Log.err("Open Platform Surface Area " + path, "Invalid file type");
            }
        }
        Sort.sortPlatforms(vPlatformList, DataType.SORT_TYPE_ASCENDING);
        return vPlatformList;
    }

    /**
     Get all Library Class Definitions from a package
     
     @return Vector
     **/
    public Vector<String> getAllLibraryClassDefinitionsFromPackage(PackageSurfaceArea spd) {
        Vector<String> vector = new Vector<String>();
        if (spd.getLibraryClassDeclarations() != null) {
            if (spd.getLibraryClassDeclarations().getLibraryClassList().size() > 0) {
                for (int index = 0; index < spd.getLibraryClassDeclarations().getLibraryClassList().size(); index++) {
                    vector.addElement(spd.getLibraryClassDeclarations().getLibraryClassList().get(index).getName());
                }
            }
        }
        Sort.sortVectorString(vector, DataType.SORT_TYPE_ASCENDING);
        return vector;
    }

    /**
     Get all Protocol Definitions from a package
     
     @return Vector
     **/
    public Vector<String> getAllProtocolDeclarationsFromPackage(PackageSurfaceArea spd) {
        Vector<String> vector = new Vector<String>();
        if (spd.getProtocolDeclarations() != null) {
            if (spd.getProtocolDeclarations().getEntryList().size() > 0) {
                for (int index = 0; index < spd.getProtocolDeclarations().getEntryList().size(); index++) {
                    vector.addElement(spd.getProtocolDeclarations().getEntryList().get(index).getCName());
                }
            }
        }
        Sort.sortVectorString(vector, DataType.SORT_TYPE_ASCENDING);
        return vector;
    }

    /**
     Get all Ppi Definitions from a package
     
     @return Vector
     **/
    public Vector<String> getAllPpiDeclarationsFromPackage(PackageSurfaceArea spd) {
        Vector<String> vector = new Vector<String>();
        if (spd.getPpiDeclarations() != null) {
            if (spd.getPpiDeclarations().getEntryList().size() > 0) {
                for (int index = 0; index < spd.getPpiDeclarations().getEntryList().size(); index++) {
                    vector.addElement(spd.getPpiDeclarations().getEntryList().get(index).getCName());
                }
            }
        }
        Sort.sortVectorString(vector, DataType.SORT_TYPE_ASCENDING);
        return vector;
    }

    /**
     Get all Guid Definitions from a package
     
     @return Vector
     **/
    public Vector<String> getAllGuidDeclarationsFromPackage(PackageSurfaceArea spd) {
        Vector<String> vector = new Vector<String>();
        if (spd.getGuidDeclarations() != null) {
            if (spd.getGuidDeclarations().getEntryList().size() > 0) {
                for (int index = 0; index < spd.getGuidDeclarations().getEntryList().size(); index++) {
                    vector.addElement(spd.getGuidDeclarations().getEntryList().get(index).getCName());
                }
            }
        }
        Sort.sortVectorString(vector, DataType.SORT_TYPE_ASCENDING);
        return vector;
    }

    /**
     Get all Pcd Definitions from a package
     
     @return Vector
     **/
    public PcdVector getAllPcdDeclarationsFromPackage(PackageSurfaceArea spd) {
        PcdVector vector = new PcdVector();
        if (spd.getPcdDeclarations() != null) {
            if (spd.getPcdDeclarations().getPcdEntryList().size() > 0) {
                for (int index = 0; index < spd.getPcdDeclarations().getPcdEntryList().size(); index++) {
                    String name = spd.getPcdDeclarations().getPcdEntryList().get(index).getCName();
                    String guidCName = spd.getPcdDeclarations().getPcdEntryList().get(index).getTokenSpaceGuidCName();
                    String help = spd.getPcdDeclarations().getPcdEntryList().get(index).getHelpText();
                    Vector<String> type = Tools.convertListToVector(spd.getPcdDeclarations().getPcdEntryList()
                                                                       .get(index).getValidUsage());

                    vector.addPcd(new PcdIdentification(name, guidCName, help, type));
                }
            }
        }
        Sort.sortPcds(vector, DataType.SORT_TYPE_ASCENDING);
        return vector;
    }

    public Vector<String> getAllLibraryClassDefinitionsFromWorkspace() {
        //
        // First get all packages
        //
        this.getAllPackages();

        Vector<String> vector = new Vector<String>();
        for (int index = 0; index < this.vPackageList.size(); index++) {
            try {
                Vector<String> v = getAllLibraryClassDefinitionsFromPackage(OpenFile
                                                                                    .openSpdFile(vPackageList
                                                                                                             .get(index)
                                                                                                             .getPath()));
                if (v != null && v.size() > 0) {
                    vector.addAll(v);
                }
            } catch (IOException e) {
                // TODO Auto-generated catch block
            } catch (XmlException e) {
                // TODO Auto-generated catch block
            } catch (Exception e) {
                // TODO Auto-generated catch block
            }
        }
        Sort.sortVectorString(vector, DataType.SORT_TYPE_ASCENDING);
        return vector;
    }

    public Vector<String> getAllProtocolDeclarationsFromWorkspace() {
        //
        // First get all packages
        //
        this.getAllPackages();

        Vector<String> vector = new Vector<String>();
        for (int index = 0; index < this.vPackageList.size(); index++) {
            try {
                Vector<String> v = getAllProtocolDeclarationsFromPackage(OpenFile.openSpdFile(vPackageList.get(index)
                                                                                                          .getPath()));
                if (v != null && v.size() > 0) {
                    vector.addAll(v);
                }
            } catch (IOException e) {
                // TODO Auto-generated catch block
            } catch (XmlException e) {
                // TODO Auto-generated catch block
            } catch (Exception e) {
                // TODO Auto-generated catch block
            }
        }
        Sort.sortVectorString(vector, DataType.SORT_TYPE_ASCENDING);
        return vector;
    }

    public Vector<String> getAllPpiDeclarationsFromWorkspace() {
        //
        // First get all packages
        //
        this.getAllPackages();

        Vector<String> vector = new Vector<String>();
        for (int index = 0; index < this.vPackageList.size(); index++) {
            try {
                Vector<String> v = getAllPpiDeclarationsFromPackage(OpenFile.openSpdFile(vPackageList.get(index)
                                                                                                     .getPath()));
                if (v != null && v.size() > 0) {
                    vector.addAll(v);
                }
            } catch (IOException e) {
                // TODO Auto-generated catch block
            } catch (XmlException e) {
                // TODO Auto-generated catch block
            } catch (Exception e) {
                // TODO Auto-generated catch block
            }
        }
        Sort.sortVectorString(vector, DataType.SORT_TYPE_ASCENDING);
        return vector;
    }

    public Vector<String> getAllGuidDeclarationsFromWorkspace() {
        //
        // First get all packages
        //
        this.getAllPackages();

        Vector<String> vector = new Vector<String>();
        for (int index = 0; index < this.vPackageList.size(); index++) {
            try {
                Vector<String> v = getAllGuidDeclarationsFromPackage(OpenFile.openSpdFile(vPackageList.get(index)
                                                                                                      .getPath()));
                if (v != null && v.size() > 0) {
                    vector.addAll(v);
                }
            } catch (IOException e) {
                // TODO Auto-generated catch block
            } catch (XmlException e) {
                // TODO Auto-generated catch block
            } catch (Exception e) {
                // TODO Auto-generated catch block
            }
        }
        Sort.sortVectorString(vector, DataType.SORT_TYPE_ASCENDING);
        return vector;
    }

    public PcdVector getAllPcdDeclarationsFromWorkspace() {
        //
        // First get all packages
        //
        this.getAllPackages();

        PcdVector vector = new PcdVector();
        for (int index = 0; index < this.vPackageList.size(); index++) {
            try {
                PcdVector v = getAllPcdDeclarationsFromPackage(OpenFile.openSpdFile(vPackageList.get(index).getPath()));
                if (v != null && v.size() > 0) {
                    vector.addAll(v);
                }
            } catch (IOException e) {
                // TODO Auto-generated catch block
            } catch (XmlException e) {
                // TODO Auto-generated catch block
            } catch (Exception e) {
                // TODO Auto-generated catch block
            }
        }
        Sort.sortPcds(vector, DataType.SORT_TYPE_ASCENDING);
        return vector;
    }

    /**
     Find a module's package's id
     
     @param id input module id
     @return package id
     
     **/
    public PackageIdentification getPackageIdByModuleId(Identification id) {
        getAllPackages();
        Vector<String> modulePaths = new Vector<String>();
        Identification mid = null;
        String packagePath = null;
        String modulePath = null;

        //
        // For each package, get all modules list
        //
        for (int indexI = 0; indexI < vPackageList.size(); indexI++) {
            packagePath = vPackageList.elementAt(indexI).getPath();
            modulePaths = this.getAllModulesOfPackage(packagePath);
            for (int indexJ = 0; indexJ < modulePaths.size(); indexJ++) {
                modulePath = modulePaths.get(indexJ);
                try {
                    mid = getId(modulePath, OpenFile.openMsaFile(modulePath));
                    //
                    // Check id
                    //
                    if (mid.equals(id)) {
                        return vPackageList.elementAt(indexI);
                    }
                } catch (IOException e) {

                } catch (XmlException e) {

                } catch (Exception e) {

                }
            }
        }

        return null;
    }

    public Identification getId(String path, ModuleSurfaceArea msa) {
        MsaHeader head = msa.getMsaHeader();
        String name = head.getModuleName();
        String guid = head.getGuidValue();
        String version = head.getVersion();
        Identification id = new Identification(name, guid, version, path);
        return id;
    }

    public Identification getId(String path, PackageSurfaceArea spd) {
        SpdHeader head = spd.getSpdHeader();
        String name = head.getPackageName();
        String guid = head.getGuidValue();
        String version = head.getVersion();
        Identification id = new Identification(name, guid, version, path);
        return id;
    }

    public Identification getId(String path, PlatformSurfaceArea fpd) {
        PlatformHeader head = fpd.getPlatformHeader();
        String name = head.getPlatformName();
        String guid = head.getGuidValue();
        String version = head.getVersion();
        Identification id = new Identification(name, guid, version, path);
        return id;
    }

    /**
     Add module information to package surface area
     
     @param mid
     @throws IOException
     @throws XmlException
     @throws Exception
     
     **/
    public void addModuleToPackage(ModuleIdentification mid, PackageSurfaceArea spd) throws IOException, XmlException,
                                                                                    Exception {
        String packagePath = mid.getPackageId().getPath();
        String modulePath = mid.getPath();
        if (spd == null) {
            spd = OpenFile.openSpdFile(packagePath);
        }
        MsaFiles msaFile = spd.getMsaFiles();
        if (msaFile == null) {
            msaFile = MsaFiles.Factory.newInstance();
        }
        packagePath = packagePath.substring(0, packagePath.lastIndexOf(DataType.FILE_SEPARATOR));
        String fn = Tools.getRelativePath(modulePath, packagePath);
        msaFile.addNewFilename();
        msaFile.setFilenameArray(msaFile.getFilenameList().size() - 1, fn);
        spd.setMsaFiles(msaFile);
        SaveFile.saveSpdFile(mid.getPackageId().getPath(), spd);
    }

    /**
     Add input package into database
     
     @param id
     * @throws Exception 
     
     **/
    public void addPackageToDatabase(PackageIdentification id) throws Exception {
        String path = id.getPath();
        path = Tools.getRelativePath(path, Workspace.getCurrentWorkspace());
        this.openFrameworkDb();
        DbPathAndFilename filename = DbPathAndFilename.Factory.newInstance();
        filename.setStringValue(path);
        fdb.getPackageList().addNewFilename();
        fdb.getPackageList().setFilenameArray(fdb.getPackageList().sizeOfFilenameArray() - 1, filename);
        String strFrameworkDbFilePath = Workspace.getCurrentWorkspace() + Workspace.getStrWorkspaceDatabaseFile();
        strFrameworkDbFilePath = Tools.convertPathToCurrentOsType(strFrameworkDbFilePath);
        SaveFile.saveDbFile(strFrameworkDbFilePath, fdb);
    }

    /**
     Add input package into database
     
     @param id
     * @throws Exception 
     
     **/
    public void addPlatformToDatabase(PlatformIdentification id) throws Exception {
        String path = id.getPath();
        path = Tools.getRelativePath(path, Workspace.getCurrentWorkspace());
        this.openFrameworkDb();
        DbPathAndFilename filename = DbPathAndFilename.Factory.newInstance();
        filename.setStringValue(path);
        fdb.getPlatformList().addNewFilename();
        fdb.getPlatformList().setFilenameArray(fdb.getPlatformList().sizeOfFilenameArray() - 1, filename);
        String strFrameworkDbFilePath = Workspace.getCurrentWorkspace() + Workspace.getStrWorkspaceDatabaseFile();
        strFrameworkDbFilePath = Tools.convertPathToCurrentOsType(strFrameworkDbFilePath);
        SaveFile.saveDbFile(strFrameworkDbFilePath, fdb);
    }

    /**
     Get all file's path from one module
     
     @param path
     @return
     @throws IOException
     @throws XmlException
     @throws Exception
     
     **/
    public Vector<String> getAllModuleFilesPath(String path) throws IOException, XmlException, Exception {
        Vector<String> v = new Vector<String>();
        path = Tools.convertPathToCurrentOsType(path);
        v.addElement(path);
        ModuleSurfaceArea msa = OpenFile.openMsaFile(path);
        if (msa != null) {
            //
            // Get all files' path of a module
            //
            SourceFiles sf = msa.getSourceFiles();
            if (sf != null) {
                for (int index = 0; index < sf.getFilenameList().size(); index++) {
                    String temp = sf.getFilenameList().get(index).getStringValue();
                    temp = Tools.addFileSeparator(Tools.getFilePathOnly(path)) + temp;
                    v.addElement(Tools.convertPathToCurrentOsType(temp));
                }
            }
        }

        return v;
    }

    /**
     Get all file's path from one package
     
     @param path
     @return
     @throws IOException
     @throws XmlException
     @throws Exception
     
     **/
    public Vector<String> getAllPakcageFilesPath(String path) throws IOException, XmlException, Exception {
        Vector<String> v = new Vector<String>();
        path = Tools.convertPathToCurrentOsType(path);
        //
        // First add package
        //
        v.addElement(path);

        //
        // Add the package's industry std includes one by one
        //
        Vector<String> f1 = new Vector<String>();
        f1 = getAllIndustryStdIncludesOfPackage(path);
        for (int index = 0; index < f1.size(); index++) {
            v.addElement(f1.get(index));
        }

        //
        // Add module's files one by one
        //
        f1 = new Vector<String>();
        f1 = getAllModulesOfPackage(path);
        for (int indexI = 0; indexI < f1.size(); indexI++) {
            Vector<String> f2 = getAllModuleFilesPath(f1.get(indexI));
            for (int indexJ = 0; indexJ < f2.size(); indexJ++) {
                v.addElement(f2.get(indexJ));
            }
        }

        return v;
    }
}
