package datespackage;

import java.rmi.Remote;
import java.rmi.RemoteException;

import java.util.Calendar;

public interface GetDatesWS
  extends Remote
{


  public byte[] getPicture(String ID, boolean isDrug)
    throws RemoteException;


  public boolean checkInteraction(String ID, String drugID)
    throws RemoteException;


  public String[] getSince(long timestamp)
    throws RemoteException;


  public String[] getUniqueUIDs()
    throws RemoteException;

  public String[] getRoute(String RFIDNum, long sinceWhen)
    throws RemoteException;

  public boolean callHome(String UID, float[] lat, float[] longit, long[] timeSinceLastReading, long[] reportedTime, float[] bearing, float[] speed, int[] elevation)
    throws RemoteException;

  public GetDates getItem(String RFIDNum, String UID, float lat, 
                          float longit)
    throws RemoteException;

  public patientInfo getPatientInfo(String rfidNum)
    throws RemoteException;

  public boolean setItem(String RFIDNum, String UID, String description, 
                         boolean authenticated)
    throws RemoteException;
}
