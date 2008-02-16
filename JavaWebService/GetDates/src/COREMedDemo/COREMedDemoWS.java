package COREMedDemo;

import java.rmi.Remote;
import java.rmi.RemoteException;

public interface COREMedDemoWS
  extends Remote
{
  public errorReport enterDrugInfo(drugInfo newDrug)
    throws RemoteException;

  public errorReport enterPatientRecord(patientRecord newPatient)
    throws RemoteException;

  public errorReport enterPatientPhoto(patientRecord newPatient)
    throws RemoteException;

  public errorReport logPatientVitals(String RFIDNum, float[] temperatures, 
    int periodicity)
    throws RemoteException;


  public boolean registerDoseGiven(String patientRFIDNum, 
                                   String drugRFIDNum, String nurseID, 
                                   int dose)
    throws RemoteException;

  public errorReport checkInteraction(String ID, String drugID)
    throws RemoteException;

  public patientRecord getPatientRecord(String RFIDNum, String UID)
    throws RemoteException;

  public drugInfo getDrugInfo(String RFIDNum, String UID)
    throws RemoteException;
}
