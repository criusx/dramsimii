package MedReminder;

import java.rmi.Remote;
import java.rmi.RemoteException;

import petPackage.errorMessage;
import petPackage.petInfo;

public interface MedReminderWebService
  extends Remote
{


  public boolean changePassword(String userID, String oldPassword, 
                                String newPassword)
    throws RemoteException;

  public medReminders getReminders(String login, String password)
    throws RemoteException;

  public drugInfo getDrug(String drugCode, String UID)
    throws RemoteException;

  public MedReminder.errorMessage addReminder(String login, 
                                              String password, 
                                              medReminders newReminder)
    throws RemoteException;
}
