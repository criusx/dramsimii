package MedReminder;

import java.rmi.Remote;
import java.rmi.RemoteException;

public interface MedReminderWebService
  extends Remote
{


  public MedReminder.errorMessage addReminder(String login, 
                                              String password, 
                                              medReminders newReminder)
    throws RemoteException;

  public boolean changePassword(String userID, String oldPassword, 
                                String newPassword)
    throws RemoteException;

  public medReminders getReminders(String login, String password)
    throws RemoteException;

  public drugInfo getDrug(String drugCode, String UID)
    throws RemoteException;

  public errorMessage delReminder(String login, String password, 
                                  medReminders newReminder)
    throws RemoteException;

  public errorMessage authorize(userInfo newUser)
    throws RemoteException;
}
