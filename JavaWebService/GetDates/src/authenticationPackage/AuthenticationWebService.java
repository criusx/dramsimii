package authenticationPackage;

import java.rmi.Remote;
import java.rmi.RemoteException;

public interface AuthenticationWebService
  extends Remote
{
  public boolean callHome(String UID, float[] lat, float[] longit, long[] timeSinceLastReading, long[] reportedTime, float[] bearing, float[] speed, int[] elevation)
    throws RemoteException;

  public String[] getSince(long timestamp)
    throws RemoteException;

  public boolean setItem(itemInfo newItem, String UID)
    throws RemoteException;

  public String[] getRoute(String RFIDNum, long sinceWhen)
    throws RemoteException;

  public itemInfo getItem(String RFIDNum, String UID, float lat, 
                          float longit)
    throws RemoteException;

  public String[] getUniqueUIDs()
    throws RemoteException;
}
