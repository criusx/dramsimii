package MedReminder;

import java.rmi.Remote;
import java.rmi.RemoteException;

import petPackage.errorMessage;
import petPackage.petInfo;

public interface MedReminderWebService
  extends Remote
{

  public errorMessage updateUserInformation(userInfo newUser)
    throws RemoteException;

  public boolean changePassword(String userID, String oldPassword, 
                                String newPassword)
    throws RemoteException;

  public medReminders getReminders(String login, String password)
    throws RemoteException;

  public errorMessage authorize(userInfo newUser)
    throws RemoteException;
}
