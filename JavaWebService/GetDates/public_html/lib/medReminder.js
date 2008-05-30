
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
            newRow = document.createElement("tr");
            
            newCell = document.createElement("td");
            newCell.appendChild(document.createTextNode(results.medicationName[i]));
            newRow.appendChild(newCell);
            
            newCell = document.createElement("td");
            newCell.appendChild(document.createTextNode(results.doses[i]));
            newRow.appendChild(newCell);
            
            newCell = document.createElement("td");
            newCell.appendChild(document.createTextNode(results.lastSent[i]));
            newRow.appendChild(newCell);
            
            newCell = document.createElement("td");
            newCell.appendChild(document.createTextNode(results.reminderFrequency[i]));
            newRow.appendChild(newCell);
            
            newCell = document.createElement("td");
            var newImage = document.createElement("img");
            newImage.setAttribute("src", "medImage.jsp?ID=" + results.ID[i]);
            newImage.setAttribute("alt", results.medicationName[i]);
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