
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
        beforeSubmit: addNewReminder
    });
    $("#newMedForm") .ajaxForm(
    {
        beforeSubmit:addMed
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
function getReminderData(login, password)
{
    $.blockUI(
    {
        message: "Please wait..."
    });
    $("#storedEmail") [0] .value = login;
    $("#storedPassword") [0] .value = password;
    var results = MedReminderWebServiceSoapHttpPort_getReminders(login, password);
    if (results.retryNeeded == true)
    {
        jQuery.blockUI(
        {
            message: "Problem logging into server, please try again later."
        });
        setTimeout(jQuery.unblockUI, 4000);
    }
    else if (results.exists == "false")
    {
        jQuery.blockUI(
        {
            message: "Login or password is incorrect."
        });
        setTimeout(jQuery.unblockUI, 4000);
    }
    else
    {
        // clear existing elements from the table
        $("tbody#tableBody") .children("tr") .remove();
        // add the results to the table
        var newRow;
        var newCell;
        for (var i = 0; i < results.medicationName.length; i++)
        {
            addReminderToTable(results.medicationName[i], results.doses[i] + " mg", results.lastSent[i], results.reminderFrequency[i], results.ID[i]);
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
    
    newUser.password = form.password.value;
    
    newUser.phoneNumber = form.phoneNumber.value;
    newUser.provider = form.provider.value;
    newUser.exists = true;
    newUser.image = [];
    var returnValue = MedReminderWebServiceSoapHttpPort_authorize(newUser);
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
        setTimeout($.unblockUI(), 4000);
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
function showLogin()
{
    $("#loginPasswordForm") [0].reset();
    jQuery.blockUI(
    {
        message: $("#loginPasswordBox")
    });
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
    
    $("#medName") .empty();
    $("#medName") .append(document.createTextNode(medInfo.name));
    
    $("#reminderDoseSelect") .empty();
    $("#reminderFrequencySelect") .empty();
    
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
    
    $("medDescription") .empty();
    $("#medDescription") .append(document.createTextNode(medInfo.description));
    
    $.blockUI(
    {
        message: $("#newMedBox")
    });
    return false;
}

function addReminderToTable(medicationName, dose, lastSent, frequency, ID)
{
    // add the results to the table
    var newRow;
    var newCell;
    
    newRow = document.createElement("tr");
    
    newCell = document.createElement("td");
    newCell.appendChild(document.createTextNode(medicationName));
    newRow.appendChild(newCell);
    
    newCell = document.createElement("td");
    newCell.appendChild(document.createTextNode(dose));
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
    newImage.setAttribute("alt", "Delete this reminder.");
    newCell.appendChild(newImage);
    newRow.appendChild(newCell);
    
    document.getElementById("tableBody") .appendChild(newRow);
    
}

function addMed(formData, jqForm, options)
{
    var form = jqForm[0];
    var newReminder =
    {
    };
    newReminder.retryNeeded = "false";
    newReminder.name = " ";
    newReminder.exists = "true";
    newReminder.lastSent = [0];
    newReminder.ID = [form.newMedID.value];
    newReminder.medicationName = [ "" ];
    newReminder.doses = [form.reminderDoseSelect.value];
    newReminder.reminderFrequency = [form.reminderFrequencySelect.value];
    var errorCode = MedReminderWebServiceSoapHttpPort_addReminder(form.storedEmail.value, form.storedPassword.value, newReminder);
    if (errorCode.success == "true")
    {
        addReminderToTable(form.storedName.value, form.reminderDoseSelect.value, "Never", form.reminderFrequencySelect.value, form.newMedID.value);
    }
    
    setTimeout($.unblockUI, 1000);
    return false;
}