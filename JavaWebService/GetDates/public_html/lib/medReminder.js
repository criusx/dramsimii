var storedLogin = null;
var storedPassword = null;
// initialize the document
$(document) .ready(function ()
{
    $("#signupForm") .ajaxForm(
    {
        beforeSubmit: processSignup
    });
    $("#loginPasswordForm") .ajaxForm(
    {
        beforeSubmit: showRequest
    });
    $("#newReminderForm") .ajaxForm(
    {
        beforeSubmit: addNewReminder,
        clearForm: true
    });
    $("#newMedForm") .ajaxForm(
    {
        beforeSubmit:addMed,
        clearForm: true
    });
});

// pre-submit callback
function showRequest(formData, jqForm, options)
{
    var form = jqForm[0];
    if ( ! form.login.value || ! form.password.value)
    {
        $.blockUI(
        {
            message: "Please enter a username and password."
        });
        setTimeout(jQuery.unblockUI, 2500);
        return false;
    }
    $.unblockUI();
    getReminderData(form.login.value, form.password.value);
    return false;
}


function showLogin()
{
    if (storedLogin == null)
    {
        // then try to log the user in
        $("#loginPasswordForm") [0].reset();
        jQuery.blockUI(
        {
            message: $("#loginPasswordBox")
        });
    }
    else
    {
        // else do logout stuff
        $("#loginText") .children("a") .empty();
        $("#loginText") .children("a") [0].appendChild(document.createTextNode("Login"));
        $("#signupText") .fadeIn("slow");
        // store login/password for future ops
        storedLogin = null;
        storedPassword = null;
        $("#remindersDiv") .slideUp("slow");
    }
}

function getReminderData(login, password)
{
    $.blockUI(
    {
        message: "Please wait..."
    });
    
    var reminderRecord = MedReminderWebServiceSoapHttpPort_getReminders(login, password);
    if (reminderRecord.retryNeeded == true)
    {
        jQuery.blockUI(
        {
            message: "Problem logging into server, please try again later."
        });
        setTimeout(jQuery.unblockUI, 4000);
    }
    else if (reminderRecord.exists == "false")
    {
        jQuery.blockUI(
        {
            message: "Login or password is incorrect."
        });
        setTimeout(jQuery.unblockUI, 4000);
    }
    else
    {
        $("#loginText") .children("a") .empty();
        $("#loginText") .children("a") [0].appendChild(document.createTextNode("Sign out"));
        $("#signupText") .fadeOut("slow");
        // store login/password for future ops
        storedLogin = login;
        storedPassword = password;
        // clear existing elements from the table
        $("tbody#tableBody") .children("tr") .remove();
        // add the results to the table
        var newRow;
        var newCell;
        for (var i = 0; i < reminderRecord.reminders.length; i++)
        {
            addReminderToTable(reminderRecord.reminders[i].medicationName, reminderRecord.reminders[i].doses, reminderRecord.reminders[i].lastSent, reminderRecord.reminders[i].reminderFrequency, reminderRecord.reminders[i].ID, "fast");
        }
        // remove the block on the screen
        jQuery.unblockUI();
        // display the table
        if ($("#signupDiv") .is(":visible"))
        {
            $("#signupDiv") .slideUp("slow");
        }
        $("#remindersDiv") .slideDown("slow");
    }
    //setTimeout(jQuery.unblockUI,4000);
    return true;
}

function processSignup(formData, jqForm, options)
{
    var form = jqForm[0];
    var filter = new RegExp("^[A-Z0-9._%+-]+@[A-Z0-9.-]+\.(?:[A-Z]{2}|COM|ORG|NET|GOV|MIL|BIZ|INFO|MOBI|NAME|AERO|JOBS|MUSEUM)$");
    if ( ! form.email.value || ! form.password.value || ! form.phoneNumber.value || ! form.provider.value || ! form.signupName.value)
    {
        $.blockUI(
        {
            message: "Please fill in all values"
        });
        setTimeout($.unblockUI, 3000);
    }
    else if ( ! filter.test(form.email.value.toUpperCase()))
    {
        $.blockUI(
        {
            message: "Please enter a valid email address"
        });
        setTimeout($.unblockUI, 4000);
        return false;
    }
    var newUser =
    {
    };
    newUser.name = form.signupName.value;
    
    newUser.email = form.email.value;
    
    newUser.phoneNumber = form.phoneNumber.value;
    
    newUser.provider = form.provider.value;
    
    newUser.image = [];
    
    var returnValue = MedReminderWebServiceSoapHttpPort_authorize(newUser, form.password.value);
    
    if (returnValue.success == "true")
    {
        // display reminders page
        getReminderData(form.email.value, form.password.value);
    }
    else
    {
        $.blockUI(
        {
            message: returnValue.errorMessage
        });
        setTimeout($.unblockUI, 4000);
    }
    return false;
}

function toggle()
{
    if ($("#remindersDiv") .is(":hidden"))
    {
        $("#remindersDiv") .slideDown("slow");
    } else
    {
        $("#remindersDiv") .slideUp("slow");
    }
}

function showSignup()
{
    if ($("#remindersDiv") .is(":visible"))
    {
        $("#remindersDiv") .slideUp("slow");
    }
    $("#signupDiv") .slideDown("slow");
}


function addReminderClick()
{
    $.blockUI(
    {
        message: $("#newReminderBox")
    });
}

function addNewReminder(formData, jqForm, options)
{
    $.blockUI(
    {
        message: "Please wait..."
    });
    
    var medInfo = MedReminderWebServiceSoapHttpPort_getDrug(jqForm[0].medID.value, "FFFFFFFB");
    
    if (medInfo.exists == "true")
    {
        $("#medName") .empty();
        $("#reminderDoseSelect") .empty();
        $("#reminderFrequencySelect") .empty();
        $("#medDescription") .empty();
        
        $("#medName") .append(document.createTextNode(medInfo.name));
        
        for each(x in medInfo.reminderIntervals)
        {
            var newOption = document.createElement("option");
            newOption.appendChild(document.createTextNode(x));
            $("#reminderFrequencySelect") .append(newOption);
        }
        
        for each(x in medInfo.doses)
        {
            var newOption = document.createElement("option");
            newOption.appendChild(document.createTextNode(x));
            $("#reminderDoseSelect") .append(newOption);
        }
        
        $("#newMedPic") [0].setAttribute("src", "medImage.jsp?ID=" + medInfo.RFIDNum);
        
        $("#newMedID") [0] .value = medInfo.RFIDNum;
        
        $("#storedName") [0].value = medInfo.name;
        
        $("#medDescription") .append(document.createTextNode(medInfo.description));
        
        $.blockUI(
        {
            message: $("#newMedBox")
        });
    }
    else
    {
        $.blockUI(
        {
            message: "Med ID not found."
        });
        setTimeout($.unblockUI, 3000);
    }
    
    return false;
}

function addReminderToTable(medicationName, dose, lastSent, frequency, ID, speed)
{
    // add the results to the table
    var newRow;
    var newCell;
    
    newRow = document.createElement("tr");
    
    newCell = document.createElement("td");
    newCell.appendChild(document.createTextNode(medicationName));
    newRow.appendChild(newCell);
    
    newCell = document.createElement("td");
    newCell.appendChild(document.createTextNode(dose + " mg"));
    newRow.appendChild(newCell);
    
    newCell = document.createElement("td");
    newCell.appendChild(document.createTextNode(lastSent));
    newRow.appendChild(newCell);
    
    newCell = document.createElement("td");
    newCell.appendChild(document.createTextNode(frequency));
    newRow.appendChild(newCell);
    
    newCell = document.createElement("td");
    var newImage = document.createElement("img");
    newImage.setAttribute("src", "medImage.jsp?ID=" + ID);
    newImage.setAttribute("alt", medicationName);
    newCell.appendChild(newImage);
    newRow.appendChild(newCell);
    
    newCell = document.createElement("td");
    var newImage = document.createElement("img");
    newImage.setAttribute("src", "images/deleteSmall.png");
    newImage.setAttribute("onclick", "javascript:deleteReminder(" + ID + "," + dose + ");");
    newImage.setAttribute("alt", "Delete this reminder.");
    newCell.appendChild(newImage);
    newRow.appendChild(newCell);
    
    newRow.setAttribute("id", ID + dose);
    
    newRow.setAttribute("style", "display:none;");
    
    document.getElementById("tableBody") .appendChild(newRow);
    
    $("#" + ID + dose) .fadeIn(speed);
}

function deleteReminder(ID, dose)
{
    var reminderToDelete =
    {
    };
    reminderToDelete.reminderPeriod = "0";
    
    reminderToDelete.medicationName = " ";
    
    reminderToDelete.ID = ID;
    
    reminderToDelete.lastSent = "0";
    
    reminderToDelete.doses = dose;
    
    var result = MedReminderWebServiceSoapHttpPort_delReminder(storedLogin, storedPassword, reminderToDelete);
    
    if (result.success == "true")
    {
        $("#" + ID + dose) .fadeOut(1500, function ()
        {
            $(this) .remove();
        });
    }
    else
    {
        $.blockUI(
        {
            message : "Could not delete reminder, please notify the sysadmin"
        });
    }
}

function addMed(formData, jqForm, options)
{
    var form = jqForm[0];
    var newReminder =
    {
    };
    newReminder.medicationName = " ";
    newReminder.lastSent = "0";
    newReminder.ID = form.newMedID.value;
    newReminder.dose = form.reminderDoseSelect.value;
    newReminder.reminderPeriod = form.reminderFrequencySelect.value;
    
    var errorCode = MedReminderWebServiceSoapHttpPort_addReminder(storedLogin, storedPassword, newReminder);
    
    if (errorCode.success == "true")
    {
        setTimeout($.unblockUI, 1000);
        addReminderToTable(form.storedName.value, form.reminderDoseSelect.value, "Never", form.reminderFrequencySelect.value, form.newMedID.value, "slow");
    }
    else
    {
        $.blockUI(
        {
            message: "Error adding reminder, please tell the sysadmin."
        });
        setTimeout($.unblockUI, 3000);
    }
    
    return false;
}