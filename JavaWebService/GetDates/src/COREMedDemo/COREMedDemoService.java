package COREMedDemo;

import dBInfo.dbConnectInfo;

import dhlDemo.SendApp;

import java.io.IOException;
import java.io.OutputStream;

import java.sql.Blob;

import java.sql.Connection;

import java.util.Date;

import java.sql.PreparedStatement;
import java.sql.ResultSet;
import java.sql.SQLException;

import java.sql.Statement;
import java.sql.Timestamp;

import java.util.ArrayList;

import java.util.Properties;

import javax.jws.WebMethod;
import javax.jws.WebService;

import oracle.jdbc.driver.OracleConnection;
import oracle.jdbc.driver.OracleResultSet;
import oracle.jdbc.driver.OracleStatement;
import oracle.jdbc.pool.OracleDataSource;

import javax.mail.*;
import javax.mail.internet.*;

import java.util.*;

import oracle.sql.BLOB;

import petPackage.petInfo;

import winepackage.wineBottle;

public class COREMedDemoService
{
  public COREMedDemoService()
  {
  }

  /**
   * Return true if the input argument character is
   * a digit, a space, or A-F.
   */
  public static final boolean isHexStringChar(char c)
  {
    return (Character.isDigit(c) || 
            (("0123456789abcdefABCDEF".indexOf(c)) >= 0));
  }

  public patientRecord getPatientRecord(String RFIDNum)
  {
    System.out.println(RFIDNum);
    if (RFIDNum == null || RFIDNum.length() < 1)
      return new patientRecord(false);

    RFIDNum = RFIDNum.replaceAll("\\s+", "");

    // if a non hex string is entered, don't try any db stuff on it
    for (int i = 0; i < RFIDNum.length(); i++)
      if (!isHexStringChar(RFIDNum.charAt(i)))
        return new patientRecord(false);

    OracleDataSource ods;

    patientRecord pR = new patientRecord();
    try
    {
      ods = new OracleDataSource();
      ods.setURL(dbConnectInfo.getConnectInfo());
      OracleConnection conn = (OracleConnection) ods.getConnection();
      conn.setAutoCommit(false);

      OracleStatement stmt = (OracleStatement) conn.createStatement();

      // set case insensitivity
      OracleStatement psC = (OracleStatement) conn.createStatement();
      psC.execute("alter session set NLS_COMP=ANSI");
      psC.execute("alter session set NLS_SORT=BINARY_CI");

      psC.close();

      PreparedStatement ps = 
        conn.prepareStatement("SELECT * FROM PATIENTDATA WHERE (ID = (SELECT PATIENTID FROM PATIENTLOOKUP WHERE PATIENTRFID = ?))");
      ps.setString(1, RFIDNum);

      ResultSet rs = (OracleResultSet) ps.executeQuery();

      // if there is no patient by this id
      if (!rs.next())
        return new patientRecord(false);

      pR.setFirstName(rs.getString("FNAME"));
      pR.setMiddleName(rs.getString("MNAME"));
      pR.setLastName(rs.getString("LNAME"));

      pR.setDOB(rs.getTimestamp("DOB").getTime());

      Blob b = rs.getBlob("PICTURE");

      pR.setImage(b.getBytes((long) 1, (int) b.length()));

      pR.setRFIDnum(RFIDNum);

      pR.setExists(true);

      ps.close();

      ps = 
          conn.prepareStatement("SELECT DRUGID FROM PATIENTALLERGY WHERE PATIENTID = (SELECT PATIENTID FROM PATIENTLOOKUP WHERE PATIENTRFID = ?)");
      ps.setString(1, RFIDNum);

      rs = ps.executeQuery();

      ArrayList<String> aL = new ArrayList<String>();

      while (rs.next())
      {
        aL.add(rs.getString("DRUGID"));
      }
      pR.setAllergies((String[]) aL.toArray(new String[aL.size()]));

      aL.clear();

      ps.close();

      ps = 
          conn.prepareStatement("SELECT DRUGID FROM PATIENTMEDICATION WHERE (PATIENTID = (SELECT PATIENTID FROM PATIENTLOOKUP WHERE PATIENTRFID = ?))");
      ps.setString(1, RFIDNum);

      rs = (OracleResultSet) ps.executeQuery();

      while (rs.next())
      {
        aL.add(rs.getString("DRUGID"));
      }

      pR.setMedications((String[]) aL.toArray(new String[aL.size()]));

      rs.next();

      ps.close();

      return pR;
    }
    catch (SQLException e)
    {
      System.out.println("exception:" + e.toString());
      return new patientRecord(false);
    }

  }

  public errorReport enterPatientRecord(patientRecord newPatient)
  {
    if (newPatient.getRFIDnum() == null)
      return new errorReport(-5, "No name entered");

    newPatient.setRFIDnum(newPatient.getRFIDnum().replaceAll("\\s+", ""));

    //    for (byte c: newPatient.getRFIDnum().getBytes())
    //    {
    //      if (!isHexStringChar(c))
    //        return new errorReport(-4, "Non hex char in RFID #");
    //    }

    OracleDataSource ods;
    try
    {
      ods = new OracleDataSource();
      ods.setURL(dbConnectInfo.getConnectInfo());
      OracleConnection conn = (OracleConnection) ods.getConnection();
      conn.setAutoCommit(false);

      OracleStatement stmt = (OracleStatement) conn.createStatement();

      //TODO: generate a patient ID and add it to patientlookup
      // insert record without picture data yet
      PreparedStatement ps = 
        conn.prepareStatement("insert into patientdata " + 
                              "(id,description, fname, mname, lname, dob, picture) " + 
                              "values(?, ?, ?, ?, ?, ?, empty_blob())");

      ps.setString(1, newPatient.getRFIDnum());
      ps.setString(2, "no description");
      ps.setString(3, newPatient.getFirstName());
      ps.setString(4, newPatient.getMiddleName());
      ps.setString(5, newPatient.getLastName());
      ps.setTimestamp(6, new Timestamp(newPatient.getDOB()));
      ps.execute();
      ps.close();

      OracleStatement psC = (OracleStatement) conn.createStatement();
      psC.execute("alter session set NLS_COMP=ANSI");
      psC.execute("alter session set NLS_SORT =BINARY_CI");

      psC.close();

      // insert allergies into table, looked up by name

      PreparedStatement ps2 = 
        conn.prepareStatement("insert into patientallergy " + 
                              "(patientid, drugid) " + 
                              "values(?, (select drugid from drugdata where (name = ?)))");

      String currentMedicine = "";
      try
      {
        for (String currentAllergy: newPatient.getAllergies())
        {
          currentMedicine = currentAllergy;

          ps2.setString(1, newPatient.getRFIDnum());
          ps2.setString(2, currentAllergy);
          ps2.execute();
        }
      }
      catch (SQLException e)
      {
        return new errorReport(-3, "No match for: " + currentMedicine);
      }

      // insert current medications, looked up by name

      PreparedStatement ps3 = 
        conn.prepareStatement("insert into patientmedication " + 
                              "(patientid, drugid) " + 
                              "values(?, (select drugid from drugdata where (UPPER(name) = UPPER(?))))");
      try
      {
        for (String currentMedication: newPatient.getMedications())
        {
          currentMedicine = currentMedication;

          ps3.setString(1, newPatient.getRFIDnum());
          ps3.setString(2, currentMedication);
          ps3.execute();
        }
      }
      catch (SQLException e)
      {
        return new errorReport(-3, "No match for: " + currentMedicine);
      }

      // if everything worked, then commit
      conn.commit();

      conn.close();
      return new errorReport(0, "Successful");
    }
    catch (SQLException e)
    {
      System.out.println(e.toString());
      System.out.println(e.fillInStackTrace());
      return new errorReport(-1, "Failed: " + e.getMessage());
    }
  }

  public errorReport logPatientVitals(String RFIDNum, float[] temperatures, 
                                      int periodicity)
  {
    OracleDataSource ods;
    try
    {
      ods = new OracleDataSource();
      ods.setURL(dbConnectInfo.getConnectInfo());
      OracleConnection conn = (OracleConnection) ods.getConnection();
      conn.setAutoCommit(false);
      PreparedStatement ps = 
        conn.prepareStatement("INSERT INTO PATIENTVITALS (RFID, RECORDTIME, TEMP) VALUES(SELECT PATIENTID FROM PATIENTLOOKUPS WHERE PATIENTRFID =?, ?, ?)");
      ps.setString(1, RFIDNum);
      Date now = new Date();

      for (int j = temperatures.length - 1; j >= 0; j--)
      {
        // TODO: if any are too high, page the doctor
        ps.setTimestamp(2, 
                        new Timestamp(now.getTime() - 1000 * periodicity * 
                                      (temperatures.length - 1 - j)));
        ps.setFloat(3, temperatures[j]);
        ps.addBatch();
      }

      // TODO: check that all were successfully entered
      int[] insertCount = ps.executeBatch();
      conn.commit();
      return new errorReport();
    }
    catch (SQLException e)
    {
      System.out.println(e.toString());
      System.out.println(e.fillInStackTrace());
      return new errorReport(-1, "Failed: " + e.getMessage());
    }
  }

  public errorReport enterPatientPhoto(patientRecord newPatient)
  {
    OracleDataSource ods;
    try
    {
      ods = new OracleDataSource();
      ods.setURL(dbConnectInfo.getConnectInfo());
      OracleConnection conn = (OracleConnection) ods.getConnection();
      conn.setAutoCommit(false);

      OracleStatement stmt = (OracleStatement) conn.createStatement();
      PreparedStatement ps2 = 
        conn.prepareStatement("SELECT * FROM patientdata WHERE id = ? FOR UPDATE");
      ps2.setString(1, newPatient.getRFIDnum());

      ResultSet rs = ps2.executeQuery();

      rs.next();
      Blob b = rs.getBlob("picture");
      b.truncate(0);
      OutputStream out = b.setBinaryStream(0L);
      out.write(newPatient.getImage(), 0, newPatient.getImage().length);
      out.flush();

      // if everything worked, then commit
      conn.commit();
      conn.close();
      return new errorReport(0, "Successful");
    }
    catch (SQLException e)
    {
      System.out.println(e.toString());
      System.out.println(e.fillInStackTrace());
      return new errorReport(-1, "Failed: " + e.getMessage());
    }
    catch (IOException e)
    {
      System.out.println(e.toString());
      return new errorReport(-1, "Failed: " + e.getMessage());
    }
  }

  public errorReport enterDrugInfo(drugInfo newDrug)
  {
    OracleDataSource ods;
    try
    {
      ods = new OracleDataSource();
      ods.setURL(dbConnectInfo.getConnectInfo());
      OracleConnection conn = (OracleConnection) ods.getConnection();
      conn.setAutoCommit(false);

      PreparedStatement ps = 
        conn.prepareStatement("insert into drugdata " + 
                              "(drugid, description, name, picture) " + 
                              "values (?, ?, ?, empty_blob())");

      ps.setString(1, newDrug.getRFIDNum());
      ps.setString(2, newDrug.getDescription());
      ps.setString(3, newDrug.getName());

      ps.execute();
      //TODO: add translation from patientlookup
      PreparedStatement ps2 = 
        conn.prepareStatement("SELECT * FROM drugdata WHERE drugid = ? FOR UPDATE");

      ps2.setString(1, newDrug.getRFIDNum());

      ResultSet rs = ps2.executeQuery();

      rs.next();
      Blob b = rs.getBlob("picture");
      b.truncate(0);
      OutputStream out = b.setBinaryStream(0L);
      out.write(newDrug.getPicture(), 0, newDrug.getPicture().length);
      out.flush();

      // if everything worked, then commit
      conn.commit();
      conn.close();
      return new errorReport(0, "Successful");
    }
    catch (SQLException e)
    {
      System.out.println(e.toString());
      System.out.println(e.fillInStackTrace());
      return new errorReport(-1, "Failed: " + e.getMessage());
    }
    catch (IOException e)
    {
      System.out.println(e.toString());
      return new errorReport(-1, "Failed: " + e.getMessage());
    }
  }

  public boolean checkInteraction(String ID, String drugID)
  {
    OracleDataSource ods;
    try
    {
      ods = new OracleDataSource();
      ods.setURL(dbConnectInfo.getConnectInfo());
      OracleConnection conn = (OracleConnection) ods.getConnection();

      // set case insensitivity
      OracleStatement psC = (OracleStatement) conn.createStatement();
      psC.execute("alter session set NLS_COMP=ANSI");
      psC.execute("alter session set NLS_SORT =BINARY_CI");

      psC.close();

      // then check to see if this is allowed
      PreparedStatement ps = 
        conn.prepareStatement("SELECT * from PATIENTALLERGY where patientid = (SELECT PATIENTID FROM PATIENTLOOKUP WHERE PATIENTRFID = ?) AND drugid = (SELECT DRUGID FROM DRUGLOOKUP WHERE DRUGRFID = ?)");
      ps.setString(1, ID);
      ps.setString(2, drugID);

      ResultSet result = ps.executeQuery();
      if (!result.next())
      {
        System.out.println("No interaction found between " + ID + " and " + 
                           drugID);
        ps = 
            conn.prepareStatement("INSERT INTO PRESCRIPTIONSGIVEN (PATIENTID, DRUGID) VALUES (SELECT PATIENTID, (SELECT DRUGID FROM DRUGLOOKUPS WHERE DRUGRFID = ?) FROM PATIENTLOOKUPS WHERE PATIENTRFID = ?)");
        ps.setString(1, ID);
        ps.setString(2, drugID);
        ps.execute();
        return false;
      }
      else
      {
        System.out.println("Interaction found between " + ID + " and " + 
                           drugID);
        ps = 
            conn.prepareStatement("INSERT INTO PRESCRIPTIONWARNINGS (PATIENTID, DRUGID) SELECT PATIENTID, (SELECT DRUGID FROM DRUGLOOKUP WHERE DRUGRFID = ?) FROM PATIENTLOOKUP WHERE PATIENTRFID = ?");
        ps.setString(1, ID);
        ps.setString(2, drugID);
        ps.execute();
        // retrieve patient name and doctor id
        ps = 
            conn.prepareStatement("SELECT FNAME, MNAME, LNAME, DOCTOR FROM PATIENTDATA WHERE ID = (SELECT PATIENTID FROM PATIENTLOOKUP WHERE PATIENTRFID = ?)");
        ps.setString(1, ID);
        result = ps.executeQuery();
        String patientName;
        String doctorID;
        if (result.next())
        {
          patientName = 
              result.getString("LNAME") + ", " + result.getString("FNAME") + 
              " " + result.getString("MNAME");
          doctorID = result.getString("DOCTOR");
        }
        else
        {
          System.out.println("Could not find patient information");
          return false;
        }
        // retrieve the doctor's name and contact info
        ps = 
            conn.prepareStatement("SELECT FNAME, LNAME, MNAME, PAGER FROM DOCTORINFO WHERE DOCTORID = ?");
        ps.setString(1, doctorID);
        result = ps.executeQuery();
        String doctorName;
        String doctorContact;
        if (result.next())
        {
          doctorName = 
              result.getString("FNAME") + " " + result.getString("MNAME") + 
              " " + result.getString("LNAME");
          doctorContact = result.getString("PAGER");
        }
        else
        {
          System.out.println("Could not find doctor information");
          return false;
        }
        // retrieve drug name
        ps = 
            conn.prepareStatement("SELECT NAME FROM DRUGDATA WHERE DRUGID = (SELECT DRUGID FROM DRUGLOOKUP WHERE DRUGRFID = ?)");
        ps.setString(1, drugID);
        result = ps.executeQuery();
        String drugName;
        if (result.next())
        {
          drugName = result.getString("NAME");
        }
        else
        {
          System.out.println("Could not find drug name.");
          return false;
        }
        try
        {
          SendApp newSender = new SendApp();
          newSender.send("smtp.gmail.com", 993, "feedback@gentag.com", 
                         doctorContact, 
                         "Drug interaction noted for patient:" + 
                         patientName, 
                         "Dr. " + doctorName + ",\nA nurse has attempted to give " + 
                         patientName + " a dose of " + drugName + ".");
          System.out.println("message sent to: " + doctorContact);
        }
        catch (MessagingException e)
        {
          // TODO
          System.out.println("ex: " + e.toString());
        }
        //catch (AddressException e)
        {
          // TODO
          //System.out.println("ex: " + e.toString());
        }
        return true;
      }
    }
    catch (SQLException e)
    {
      System.out.println("Exception: " + e.toString());
      e.printStackTrace();
      return false;
    }
  }


  public drugInfo getDrugInfo(String RFIDNum)
  {
    if (RFIDNum == null)
      return new drugInfo(false);

    RFIDNum = RFIDNum.replaceAll("\\s+", "");

    // if a non hex string is entered, don't try any db stuff on it
    for (int i = 0; i < RFIDNum.length(); i++)
      if (!isHexStringChar(RFIDNum.charAt(i)))
        return new drugInfo(false);

    OracleDataSource ods;

    drugInfo pR = new drugInfo();
    try
    {
      ods = new OracleDataSource();
      ods.setURL(dbConnectInfo.getConnectInfo());
      OracleConnection conn = (OracleConnection) ods.getConnection();
      conn.setAutoCommit(false);

      OracleStatement stmt = (OracleStatement) conn.createStatement();

      // set case insensitivity
      OracleStatement psC = (OracleStatement) conn.createStatement();
      psC.execute("alter session set NLS_COMP=ANSI");
      psC.execute("alter session set NLS_SORT=BINARY_CI");

      psC.close();

      PreparedStatement ps = 
        conn.prepareStatement("SELECT * FROM DRUGDATA WHERE (DRUGID = (SELECT DRUGID FROM DRUGLOOKUP WHERE DRUGRFID = ?))");
      ps.setString(1, RFIDNum);

      ResultSet rs = (OracleResultSet) ps.executeQuery();

      // if there is no patient by this id
      if (!rs.next())
        return new drugInfo(false);
      else
      {
        pR.setRFIDNum(RFIDNum);
        pR.setDescription(rs.getString("DESCRIPTION"));
        pR.setExists(true);

        pR.setName(rs.getString("NAME"));

        Blob b = rs.getBlob("PICTURE");

        pR.setPicture(b.getBytes((long) 1, (int) b.length()));

        ps.close();

        return pR;
      }
    }
    catch (SQLException e)
    {
      System.out.println("exception:" + e.toString());
      return new drugInfo(false);
    }
  }

  public boolean registerDoseGiven(String patientRFIDNum, 
                                   String drugRFIDNum, String nurseID, 
                                   int dose)
  {
    if (patientRFIDNum == null)
      return false;

    patientRFIDNum = patientRFIDNum.replaceAll("\\s+", "");

    // if a non hex string is entered, don't try any db stuff on it
    for (int i = 0; i < patientRFIDNum.length(); i++)
      if (!isHexStringChar(patientRFIDNum.charAt(i)))
        return false;

    OracleDataSource ods;
    try
    {
      ods = new OracleDataSource();
      ods.setURL(dbConnectInfo.getConnectInfo());
      Connection conn = ods.getConnection();
      Statement stmt = conn.createStatement();
      conn.setAutoCommit(false);
      PreparedStatement ps = 
        conn.prepareStatement("INSERT INTO MEDICATIONSDISPENSED " + 
                              "(DRUGID, PATIENTID, DOSE, TIMEGIVEN, NURSEID)" + 
                              "VALUES (SELECT DRUGID FROM DRUGLOOKUP WHERE DRUGRFID = ?, SELECT PATIENTID FROM PATIENTLOOKUP WHERE PATIENTRFID = ?, ?, ?, ?)");
      ps.setString(1, drugRFIDNum);
      ps.setString(2, patientRFIDNum);
      ps.setInt(3, dose);
      ps.setTimestamp(4, new Timestamp((new java.util.Date()).getTime()));
      ps.setString(5, nurseID);

      // TODO: add lots more checks to see if this medication should not be given to this patient

      ps.execute();

      conn.commit();

      return true;
    }
    catch (SQLException ex)
    {
      System.out.println("exception: " + ex.toString());
      return false;
    }
  }
}
