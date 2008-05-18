package authenticationPackage;

import java.rmi.Remote;
import java.rmi.RemoteException;


public interface AuthenticationWebService
  extends Remote
{

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

  public boolean sendError(String exception, String stackTrace, 
                           String message)
    throws RemoteException;
}
