package authenticationPackage;

import java.rmi.Remote;
import java.rmi.RemoteException;


public interface AuthenticationSecureWS
  extends Remote
{
  public boolean setItem(itemInfo newItem, String UID)
    throws RemoteException;
}
