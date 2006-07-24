/** @file
 
 The file is used to create, update BootModes of MSA/MBD file
 
 Copyright (c) 2006, Intel Corporation
 All rights reserved. This program and the accompanying materials
 are licensed and made available under the terms and conditions of the BSD License
 which accompanies this distribution.  The full text of the license may be found at
 http://opensource.org/licenses/bsd-license.php
 
 THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
 WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
 
 **/

package org.tianocore.frameworkwizard.module.ui.dialog;

import java.awt.event.ActionEvent;
import java.util.Vector;

import javax.swing.JButton;
import javax.swing.JComboBox;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JTextField;

import org.tianocore.frameworkwizard.common.DataType;
import org.tianocore.frameworkwizard.common.DataValidation;
import org.tianocore.frameworkwizard.common.EnumerationData;
import org.tianocore.frameworkwizard.common.Log;
import org.tianocore.frameworkwizard.common.Tools;
import org.tianocore.frameworkwizard.common.ui.ArchCheckBox;
import org.tianocore.frameworkwizard.common.ui.IDialog;
import org.tianocore.frameworkwizard.common.ui.IFrame;
import org.tianocore.frameworkwizard.common.ui.StarLabel;
import org.tianocore.frameworkwizard.module.Identifications.BootModes.BootModesIdentification;

/**
 The class is used to create, update BootModes of MSA/MBD file
 It extends IDialog
 


 **/
public class BootModesDlg extends IDialog {

    ///
    /// Define class Serial Version UID
    ///
    private static final long serialVersionUID = -3888558623432442561L;

    //
    //Define class members
    //
    private JPanel jContentPane = null;

    private JLabel jLabelBootModeName = null;

    private JComboBox jComboBoxBootModeName = null;

    private JLabel jLabelUsage = null;

    private JComboBox jComboBoxUsage = null;

    private StarLabel jStarLabel1 = null;

    private StarLabel jStarLabel2 = null;

    private JLabel jLabelFeatureFlag = null;

    private JTextField jTextFieldFeatureFlag = null;

    private JLabel jLabelArch = null;

    private JScrollPane jScrollPane = null;

    private JLabel jLabelHelpText = null;

    private JTextField jTextFieldHelpText = null;

    private ArchCheckBox jArchCheckBox = null;

    private JButton jButtonOk = null;

    private JButton jButtonCancel = null;

    //
    // Not used by UI
    //
    private BootModesIdentification id = null;

    private EnumerationData ed = new EnumerationData();

    /**
     This method initializes jComboBoxBootModeName 
     
     @return javax.swing.JComboBox jComboBoxBootModeName
     
     **/
    private JComboBox getJComboBoxBootModeName() {
        if (jComboBoxBootModeName == null) {
            jComboBoxBootModeName = new JComboBox();
            jComboBoxBootModeName.setBounds(new java.awt.Rectangle(160, 10, 320, 20));
            jComboBoxBootModeName.setPreferredSize(new java.awt.Dimension(320, 20));
        }
        return jComboBoxBootModeName;
    }

    /**
     This method initializes jComboBoxUsage 
     
     @return javax.swing.JComboBox jComboBoxUsage
     
     **/
    private JComboBox getJComboBoxUsage() {
        if (jComboBoxUsage == null) {
            jComboBoxUsage = new JComboBox();
            jComboBoxUsage.setBounds(new java.awt.Rectangle(160, 35, 320, 20));
            jComboBoxUsage.setPreferredSize(new java.awt.Dimension(320, 20));
        }
        return jComboBoxUsage;
    }

    /**
     * This method initializes jTextFieldFeatureFlag	
     * 	
     * @return javax.swing.JTextField	
     */
    private JTextField getJTextFieldFeatureFlag() {
        if (jTextFieldFeatureFlag == null) {
            jTextFieldFeatureFlag = new JTextField();
            jTextFieldFeatureFlag.setBounds(new java.awt.Rectangle(160, 85, 320, 20));
            jTextFieldFeatureFlag.setPreferredSize(new java.awt.Dimension(320, 20));
        }
        return jTextFieldFeatureFlag;
    }

    /**
     This method initializes jScrollPane  
     
     @return javax.swing.JScrollPane  
     */
    private JScrollPane getJScrollPane() {
        if (jScrollPane == null) {
            jScrollPane = new JScrollPane();
            jScrollPane.setViewportView(getJContentPane());
        }
        return jScrollPane;
    }

    /**
     This method initializes jTextFieldHelpText  
     
     @return javax.swing.JTextField  
     
     **/
    private JTextField getJTextFieldHelpText() {
        if (jTextFieldHelpText == null) {
            jTextFieldHelpText = new JTextField();
            jTextFieldHelpText.setBounds(new java.awt.Rectangle(160, 60, 320, 20));
            jTextFieldHelpText.setPreferredSize(new java.awt.Dimension(320, 20));
        }
        return jTextFieldHelpText;
    }

    /**
     This method initializes jButtonOk    
     
     @return javax.swing.JButton  
     
     **/
    private JButton getJButtonOk() {
        if (jButtonOk == null) {
            jButtonOk = new JButton();
            jButtonOk.setBounds(new java.awt.Rectangle(290, 140, 90, 20));
            jButtonOk.setText("Ok");
            jButtonOk.addActionListener(this);
        }
        return jButtonOk;
    }

    /**
     This method initializes jButtonCancel    
     
     @return javax.swing.JButton  
     
     **/
    private JButton getJButtonCancel() {
        if (jButtonCancel == null) {
            jButtonCancel = new JButton();
            jButtonCancel.setBounds(new java.awt.Rectangle(390, 140, 90, 20));
            jButtonCancel.setText("Cancel");
            jButtonCancel.addActionListener(this);
        }
        return jButtonCancel;
    }

    public static void main(String[] args) {
    }

    /**
     This method initializes this
     
     **/
    private void init() {
        this.setSize(500, 200);
        this.setContentPane(getJScrollPane());
        this.setTitle("Boot Modes");
        initFrame();
        this.setViewMode(false);
        this.centerWindow();
    }

    /**
     This method initializes this
     Fill values to all fields if these values are not empty
     
     @param inBootModesId

     **/
    private void init(BootModesIdentification inBootModesId) {
        init();
        this.id = inBootModesId;

        if (this.id != null) {
            this.jComboBoxBootModeName.setSelectedItem(id.getName());
            this.jComboBoxUsage.setSelectedItem(id.getUsage());
            this.jTextFieldHelpText.setText(id.getHelp());
            this.jTextFieldFeatureFlag.setText(id.getFeatureFlag());
            this.jArchCheckBox.setSelectedItems(id.getSupArchList());
        }
    }

    /**
     This is the override edit constructor
     
     @param inBootModesIdentification
     @param iFrame
     
     **/
    public BootModesDlg(BootModesIdentification inBootModesIdentification, IFrame iFrame) {
        super(iFrame, true);
        init(inBootModesIdentification);
    }

    /**
     Disable all components when the mode is view
     
     @param isView true - The view mode; false - The non-view mode
     
     **/
    public void setViewMode(boolean isView) {
        if (isView) {
            this.jComboBoxBootModeName.setEnabled(!isView);
            this.jComboBoxUsage.setEnabled(!isView);
        }
    }

    /**
     This method initializes jContentPane
     
     @return javax.swing.JPanel jContentPane
     
     **/
    private JPanel getJContentPane() {
        if (jContentPane == null) {
            jArchCheckBox = new ArchCheckBox();
            jArchCheckBox.setBounds(new java.awt.Rectangle(160, 110, 320, 20));
            jArchCheckBox.setPreferredSize(new java.awt.Dimension(320, 20));
            jLabelArch = new JLabel();
            jLabelArch.setBounds(new java.awt.Rectangle(15, 110, 140, 20));
            jLabelArch.setText("Arch");
            jLabelFeatureFlag = new JLabel();
            jLabelFeatureFlag.setBounds(new java.awt.Rectangle(15, 85, 140, 20));
            jLabelFeatureFlag.setText("Feature Flag");
            jLabelUsage = new JLabel();
            jLabelUsage.setText("Usage");
            jLabelUsage.setBounds(new java.awt.Rectangle(15, 35, 140, 20));
            jLabelBootModeName = new JLabel();
            jLabelBootModeName.setText("Boot Mode Name");
            jLabelBootModeName.setBounds(new java.awt.Rectangle(15, 10, 140, 20));
            jLabelHelpText = new JLabel();
            jLabelHelpText.setBounds(new java.awt.Rectangle(14, 60, 140, 20));
            jLabelHelpText.setText("Help Text");

            jContentPane = new JPanel();
            jContentPane.setLayout(null);
            jContentPane.setPreferredSize(new java.awt.Dimension(490, 165));

            jContentPane.add(jLabelBootModeName, null);
            jContentPane.add(getJComboBoxBootModeName(), null);
            jContentPane.add(jLabelUsage, null);
            jContentPane.add(getJComboBoxUsage(), null);
            jStarLabel1 = new StarLabel();
            jStarLabel1.setLocation(new java.awt.Point(0, 10));
            jStarLabel2 = new StarLabel();
            jStarLabel2.setLocation(new java.awt.Point(0, 35));

            jContentPane.add(jStarLabel1, null);
            jContentPane.add(jStarLabel2, null);
            jContentPane.add(jLabelFeatureFlag, null);
            jContentPane.add(getJTextFieldFeatureFlag(), null);
            jContentPane.add(jLabelArch, null);

            jContentPane.add(jLabelHelpText, null);
            jContentPane.add(getJTextFieldHelpText(), null);
            jContentPane.add(jArchCheckBox, null);
            jContentPane.add(getJButtonOk(), null);
            jContentPane.add(getJButtonCancel(), null);
        }
        return jContentPane;
    }

    /**
     This method initializes BootModeName groups and Usage type
     
     **/
    private void initFrame() {
        Tools.generateComboBoxByVector(jComboBoxBootModeName, ed.getVBootModeNames());
        Tools.generateComboBoxByVector(jComboBoxUsage, ed.getVPpiUsage());
    }

    /* (non-Javadoc)
     * @see java.awt.event.ActionListener#actionPerformed(java.awt.event.ActionEvent)
     *
     * Override actionPerformed to listen all actions
     * 
     */
    public void actionPerformed(ActionEvent arg0) {
        if (arg0.getSource() == jButtonOk) {
            if (checkAdd()) {
                getCurrentBootModes();
                this.returnType = DataType.RETURN_TYPE_OK;
                this.setVisible(false);
            }
        }

        if (arg0.getSource() == jButtonCancel) {
            this.returnType = DataType.RETURN_TYPE_CANCEL;
            this.setVisible(false);
        }
    }

    /**
     Data validation for all fields
     
     @retval true - All datas are valid
     @retval false - At least one data is invalid
     
     **/
    public boolean checkAdd() {
        //
        // Check if all fields have correct data types 
        //

        //
        // Check FeatureFlag
        //
        if (!isEmpty(this.jTextFieldFeatureFlag.getText())) {
            if (!DataValidation.isFeatureFlag(this.jTextFieldFeatureFlag.getText())) {
                Log.wrn("Update Boot Modes", "Incorrect data type for Feature Flag");
                return false;
            }
        }

        return true;
    }

    private BootModesIdentification getCurrentBootModes() {
        String arg0 = this.jComboBoxBootModeName.getSelectedItem().toString();

        String arg1 = this.jComboBoxUsage.getSelectedItem().toString();

        String arg2 = this.jTextFieldFeatureFlag.getText();
        Vector<String> arg3 = this.jArchCheckBox.getSelectedItemsVector();
        String arg4 = this.jTextFieldHelpText.getText();
        id = new BootModesIdentification(arg0, arg1, arg2, arg3, arg4);
        return id;
    }

    public BootModesIdentification getId() {
        return id;
    }

    public void setId(BootModesIdentification id) {
        this.id = id;
    }
}
