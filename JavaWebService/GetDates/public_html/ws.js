function firstChildElement(node)
{
    if ( ! node) return null;
var child = node.firstChild;
    while (child)
    {
        if (child.nodeType == 1)
        return child;
        child = child.nextSibling;
    }
    return null;
}

function nextSiblingElement(node)
{
    if ( ! node) return null;
var sibling = node.nextSibling;
    while (sibling)
    {
        if (sibling.nodeType == 1)
        return sibling;
        sibling = sibling.nextSibling;
    }
    return null;
}

function getText(node)
{
    if ( ! node) return null;
var text = '';
    var child = node.firstChild;
    while (child)
    {
        if (child.nodeType == 3)
        {
            text = text + child.nodeValue;
        }
        child = child.nextSibling;
    }
    return text;
}

function invokeSync(url, xmlDoc)
{
    var req = null;
if (window.XMLHttpRequest)
    {
        req = new XMLHttpRequest();
    } else if (window.ActiveXObject)
    {
        req = new ActiveXObject("Microsoft.XMLHTTP");
    }
    
    if (req)
    {
        req.open("POST", url, false);
        req.setRequestHeader("Content-Type", "text/xml");
        req.send(xmlDoc);
        return req.responseXML;
    }
}

function invokeAsync(url, xmlDoc, callback)
{
    var req = null;
    if (window.XMLHttpRequest)
    {
        req = new XMLHttpRequest();
    } else if (window.ActiveXObject)
    {
        req = new ActiveXObject("Microsoft.XMLHTTP");
    }
    
    if (req)
    {
        req.onreadystatechange = function ()
        {
            if (req.readyState == 4)
            {
                if (req.status == 200)
                {
                    callback(req.responseXML);
                }
            }
        }
        req.open("POST", url, true);
        req.setRequestHeader("Content-Type", "text/xml");
        req.send(xmlDoc);
    }
}

function createNewDocument()
{
    var xmlDoc = null;
    if (document.implementation && document.implementation.createDocument)
    {
        xmlDoc = document.implementation.createDocument("", "", null);
    } else if (window.ActiveXObject)
    {
        xmlDoc = new ActiveXObject("Microsoft.XMLDOM");
    }
    return xmlDoc;
}

function createElementNS(xmlDoc, namespace, localName)
{
    var element = null;
    if (typeof xmlDoc.createElementNS != 'undefined')
    {
        element = xmlDoc.createElementNS(namespace, localName);
    }
    else if (typeof xmlDoc.createNode != 'undefined')
    {
        if (namespace)
        {
            element = xmlDoc.createNode(1, localName, namespace);
        } else
        {
            element = xmlDoc.createElement(localName);
        }
    }
    return element;
}

function localName(element)
{
    if (element.localName)
    return element.localName;
    else
    return element.baseName;
}



function GetDatesWSSoapHttpPort_getDescription(_RFIDNum)
{
    var xmlDoc = createNewDocument();
    var envelope = createElementNS(xmlDoc, 'http://schemas.xmlsoap.org/soap/envelope/', 'Envelope');
    xmlDoc.appendChild(envelope);
    var body = createElementNS(xmlDoc, 'http://schemas.xmlsoap.org/soap/envelope/', 'Body');
    envelope.appendChild(body);
    var parameterParent = body;
    parameterParent = createElementNS(xmlDoc, 'http://datespackage/', 'getDescription');
    body.appendChild(parameterParent);
    var paramEl = createElementNS(xmlDoc, 'http://datespackage/', 'RFIDNum');
    paramEl.appendChild(xmlDoc.createTextNode(_RFIDNum));
    parameterParent.appendChild(paramEl);
    var responseDoc = invokeSync('http://crius.dyndns.org:8888/JavaWebService-GetDates-context-root/GetDatesWSSoapHttpPort', xmlDoc);
    var resultObj = null;
    body = firstChildElement(responseDoc.documentElement);
    if (localName(body) != 'Body')
    {
        body = nextSiblingElement(body);
    }
    
    var resultEl = firstChildElement(body);
    resultEl = firstChildElement(resultEl);
    resultObj = getText(resultEl);
    return resultObj;
}

function GetDatesWSSoapHttpPort_getDescriptionAsync(_RFIDNum, callback)
{
    var xmlDoc = createNewDocument();
    var envelope = createElementNS(xmlDoc, 'http://schemas.xmlsoap.org/soap/envelope/', 'Envelope');
    xmlDoc.appendChild(envelope);
    var body = createElementNS(xmlDoc, 'http://schemas.xmlsoap.org/soap/envelope/', 'Body');
    envelope.appendChild(body);
    var parameterParent = body;
    parameterParent = createElementNS(xmlDoc, 'http://datespackage/', 'getDescription');
    body.appendChild(parameterParent);
    var paramEl = createElementNS(xmlDoc, 'http://datespackage/', 'RFIDNum');
    paramEl.appendChild(xmlDoc.createTextNode(_RFIDNum));
    parameterParent.appendChild(paramEl);
    var resultsProcessor = function (responseDoc)
    {
        var resultsObj = null;
        body = firstChildElement(responseDoc.documentElement);
        if (localName(body) != 'Body')
        {
            body = nextSiblingElement(body);
        }
        
        var resultEl = firstChildElement(body);
        resultEl = firstChildElement(resultEl);
        resultObj = getText(resultEl);
        callback(resultObj);
    }
    invokeAsync('http://crius.dyndns.org:8888/JavaWebService-GetDates-context-root/GetDatesWSSoapHttpPort', xmlDoc, resultsProcessor);
}

function GetDatesWSSoapHttpPort_getInfo(_ID, _isDrug)
{
    var xmlDoc = createNewDocument();
    var envelope = createElementNS(xmlDoc, 'http://schemas.xmlsoap.org/soap/envelope/', 'Envelope');
    xmlDoc.appendChild(envelope);
    var body = createElementNS(xmlDoc, 'http://schemas.xmlsoap.org/soap/envelope/', 'Body');
    envelope.appendChild(body);
    var parameterParent = body;
    parameterParent = createElementNS(xmlDoc, 'http://datespackage/', 'getInfo');
    body.appendChild(parameterParent);
    var paramEl = createElementNS(xmlDoc, 'http://datespackage/', 'ID');
    paramEl.appendChild(xmlDoc.createTextNode(_ID));
    parameterParent.appendChild(paramEl);
    var paramEl = createElementNS(xmlDoc, 'http://datespackage/', 'isDrug');
    paramEl.appendChild(xmlDoc.createTextNode(_isDrug));
    parameterParent.appendChild(paramEl);
    var responseDoc = invokeSync('http://crius.dyndns.org:8888/JavaWebService-GetDates-context-root/GetDatesWSSoapHttpPort', xmlDoc);
    var resultObj = null;
    body = firstChildElement(responseDoc.documentElement);
    if (localName(body) != 'Body')
    {
        body = nextSiblingElement(body);
    }
    
    var resultEl = firstChildElement(body);
    resultEl = firstChildElement(resultEl);
    if (resultEl)
    resultObj = GetDatesWSSoapHttpPort_deserialize_GetDates(resultEl);
    else
    resultObj = null;
    return resultObj;
}

function GetDatesWSSoapHttpPort_getInfoAsync(_ID, _isDrug, callback)
{
    var xmlDoc = createNewDocument();
    var envelope = createElementNS(xmlDoc, 'http://schemas.xmlsoap.org/soap/envelope/', 'Envelope');
    xmlDoc.appendChild(envelope);
    var body = createElementNS(xmlDoc, 'http://schemas.xmlsoap.org/soap/envelope/', 'Body');
    envelope.appendChild(body);
    var parameterParent = body;
    parameterParent = createElementNS(xmlDoc, 'http://datespackage/', 'getInfo');
    body.appendChild(parameterParent);
    var paramEl = createElementNS(xmlDoc, 'http://datespackage/', 'ID');
    paramEl.appendChild(xmlDoc.createTextNode(_ID));
    parameterParent.appendChild(paramEl);
    var paramEl = createElementNS(xmlDoc, 'http://datespackage/', 'isDrug');
    paramEl.appendChild(xmlDoc.createTextNode(_isDrug));
    parameterParent.appendChild(paramEl);
    var resultsProcessor = function (responseDoc)
    {
        var resultsObj = null;
        body = firstChildElement(responseDoc.documentElement);
        if (localName(body) != 'Body')
        {
            body = nextSiblingElement(body);
        }
        
        var resultEl = firstChildElement(body);
        resultEl = firstChildElement(resultEl);
        if (resultEl)
        resultObj = GetDatesWSSoapHttpPort_deserialize_GetDates(resultEl);
        else
        resultObj = null;
        callback(resultObj);
    }
    invokeAsync('http://crius.dyndns.org:8888/JavaWebService-GetDates-context-root/GetDatesWSSoapHttpPort', xmlDoc, resultsProcessor);
}

function GetDatesWSSoapHttpPort_getPicture(_ID, _isDrug)
{
    var xmlDoc = createNewDocument();
    var envelope = createElementNS(xmlDoc, 'http://schemas.xmlsoap.org/soap/envelope/', 'Envelope');
    xmlDoc.appendChild(envelope);
    var body = createElementNS(xmlDoc, 'http://schemas.xmlsoap.org/soap/envelope/', 'Body');
    envelope.appendChild(body);
    var parameterParent = body;
    parameterParent = createElementNS(xmlDoc, 'http://datespackage/', 'getPicture');
    body.appendChild(parameterParent);
    var paramEl = createElementNS(xmlDoc, 'http://datespackage/', 'ID');
    paramEl.appendChild(xmlDoc.createTextNode(_ID));
    parameterParent.appendChild(paramEl);
    var paramEl = createElementNS(xmlDoc, 'http://datespackage/', 'isDrug');
    paramEl.appendChild(xmlDoc.createTextNode(_isDrug));
    parameterParent.appendChild(paramEl);
    var responseDoc = invokeSync('http://crius.dyndns.org:8888/JavaWebService-GetDates-context-root/GetDatesWSSoapHttpPort', xmlDoc);
    var resultObj = null;
    body = firstChildElement(responseDoc.documentElement);
    if (localName(body) != 'Body')
    {
        body = nextSiblingElement(body);
    }
    
    var resultEl = firstChildElement(body);
    resultEl = firstChildElement(resultEl);
    resultObj = getText(resultEl);
    return resultObj;
}

function GetDatesWSSoapHttpPort_getPictureAsync(_ID, _isDrug, callback)
{
    var xmlDoc = createNewDocument();
    var envelope = createElementNS(xmlDoc, 'http://schemas.xmlsoap.org/soap/envelope/', 'Envelope');
    xmlDoc.appendChild(envelope);
    var body = createElementNS(xmlDoc, 'http://schemas.xmlsoap.org/soap/envelope/', 'Body');
    envelope.appendChild(body);
    var parameterParent = body;
    parameterParent = createElementNS(xmlDoc, 'http://datespackage/', 'getPicture');
    body.appendChild(parameterParent);
    var paramEl = createElementNS(xmlDoc, 'http://datespackage/', 'ID');
    paramEl.appendChild(xmlDoc.createTextNode(_ID));
    parameterParent.appendChild(paramEl);
    var paramEl = createElementNS(xmlDoc, 'http://datespackage/', 'isDrug');
    paramEl.appendChild(xmlDoc.createTextNode(_isDrug));
    parameterParent.appendChild(paramEl);
    var resultsProcessor = function (responseDoc)
    {
        var resultsObj = null;
        body = firstChildElement(responseDoc.documentElement);
        if (localName(body) != 'Body')
        {
            body = nextSiblingElement(body);
        }
        
        var resultEl = firstChildElement(body);
        resultEl = firstChildElement(resultEl);
        resultObj = getText(resultEl);
        callback(resultObj);
    }
    invokeAsync('http://crius.dyndns.org:8888/JavaWebService-GetDates-context-root/GetDatesWSSoapHttpPort', xmlDoc, resultsProcessor);
}

function GetDatesWSSoapHttpPort_isAuthenticated(_RFIDNum, _UID, _lat, _longit)
{
    var xmlDoc = createNewDocument();
    var envelope = createElementNS(xmlDoc, 'http://schemas.xmlsoap.org/soap/envelope/', 'Envelope');
    xmlDoc.appendChild(envelope);
    var body = createElementNS(xmlDoc, 'http://schemas.xmlsoap.org/soap/envelope/', 'Body');
    envelope.appendChild(body);
    var parameterParent = body;
    parameterParent = createElementNS(xmlDoc, 'http://datespackage/', 'isAuthenticated');
    body.appendChild(parameterParent);
    var paramEl = createElementNS(xmlDoc, 'http://datespackage/', 'RFIDNum');
    paramEl.appendChild(xmlDoc.createTextNode(_RFIDNum));
    parameterParent.appendChild(paramEl);
    var paramEl = createElementNS(xmlDoc, 'http://datespackage/', 'UID');
    paramEl.appendChild(xmlDoc.createTextNode(_UID));
    parameterParent.appendChild(paramEl);
    var paramEl = createElementNS(xmlDoc, 'http://datespackage/', 'lat');
    paramEl.appendChild(xmlDoc.createTextNode(_lat));
    parameterParent.appendChild(paramEl);
    var paramEl = createElementNS(xmlDoc, 'http://datespackage/', 'longit');
    paramEl.appendChild(xmlDoc.createTextNode(_longit));
    parameterParent.appendChild(paramEl);
    var responseDoc = invokeSync('http://crius.dyndns.org:8888/JavaWebService-GetDates-context-root/GetDatesWSSoapHttpPort', xmlDoc);
    var resultObj = null;
    body = firstChildElement(responseDoc.documentElement);
    if (localName(body) != 'Body')
    {
        body = nextSiblingElement(body);
    }
    
    var resultEl = firstChildElement(body);
    resultEl = firstChildElement(resultEl);
    resultObj = getText(resultEl);
    return resultObj;
}

function GetDatesWSSoapHttpPort_isAuthenticatedAsync(_RFIDNum, _UID, _lat, _longit, callback)
{
    var xmlDoc = createNewDocument();
    var envelope = createElementNS(xmlDoc, 'http://schemas.xmlsoap.org/soap/envelope/', 'Envelope');
    xmlDoc.appendChild(envelope);
    var body = createElementNS(xmlDoc, 'http://schemas.xmlsoap.org/soap/envelope/', 'Body');
    envelope.appendChild(body);
    var parameterParent = body;
    parameterParent = createElementNS(xmlDoc, 'http://datespackage/', 'isAuthenticated');
    body.appendChild(parameterParent);
    var paramEl = createElementNS(xmlDoc, 'http://datespackage/', 'RFIDNum');
    paramEl.appendChild(xmlDoc.createTextNode(_RFIDNum));
    parameterParent.appendChild(paramEl);
    var paramEl = createElementNS(xmlDoc, 'http://datespackage/', 'UID');
    paramEl.appendChild(xmlDoc.createTextNode(_UID));
    parameterParent.appendChild(paramEl);
    var paramEl = createElementNS(xmlDoc, 'http://datespackage/', 'lat');
    paramEl.appendChild(xmlDoc.createTextNode(_lat));
    parameterParent.appendChild(paramEl);
    var paramEl = createElementNS(xmlDoc, 'http://datespackage/', 'longit');
    paramEl.appendChild(xmlDoc.createTextNode(_longit));
    parameterParent.appendChild(paramEl);
    var resultsProcessor = function (responseDoc)
    {
        var resultsObj = null;
        body = firstChildElement(responseDoc.documentElement);
        if (localName(body) != 'Body')
        {
            body = nextSiblingElement(body);
        }
        
        var resultEl = firstChildElement(body);
        resultEl = firstChildElement(resultEl);
        resultObj = getText(resultEl);
        callback(resultObj);
    }
    invokeAsync('http://crius.dyndns.org:8888/JavaWebService-GetDates-context-root/GetDatesWSSoapHttpPort', xmlDoc, resultsProcessor);
}

function GetDatesWSSoapHttpPort_checkInteraction(_ID, _drugID)
{
    var xmlDoc = createNewDocument();
    var envelope = createElementNS(xmlDoc, 'http://schemas.xmlsoap.org/soap/envelope/', 'Envelope');
    xmlDoc.appendChild(envelope);
    var body = createElementNS(xmlDoc, 'http://schemas.xmlsoap.org/soap/envelope/', 'Body');
    envelope.appendChild(body);
    var parameterParent = body;
    parameterParent = createElementNS(xmlDoc, 'http://datespackage/', 'checkInteraction');
    body.appendChild(parameterParent);
    var paramEl = createElementNS(xmlDoc, 'http://datespackage/', 'ID');
    paramEl.appendChild(xmlDoc.createTextNode(_ID));
    parameterParent.appendChild(paramEl);
    var paramEl = createElementNS(xmlDoc, 'http://datespackage/', 'drugID');
    paramEl.appendChild(xmlDoc.createTextNode(_drugID));
    parameterParent.appendChild(paramEl);
    var responseDoc = invokeSync('http://crius.dyndns.org:8888/JavaWebService-GetDates-context-root/GetDatesWSSoapHttpPort', xmlDoc);
    var resultObj = null;
    body = firstChildElement(responseDoc.documentElement);
    if (localName(body) != 'Body')
    {
        body = nextSiblingElement(body);
    }
    
    var resultEl = firstChildElement(body);
    resultEl = firstChildElement(resultEl);
    resultObj = getText(resultEl);
    return resultObj;
}

function GetDatesWSSoapHttpPort_checkInteractionAsync(_ID, _drugID, callback)
{
    var xmlDoc = createNewDocument();
    var envelope = createElementNS(xmlDoc, 'http://schemas.xmlsoap.org/soap/envelope/', 'Envelope');
    xmlDoc.appendChild(envelope);
    var body = createElementNS(xmlDoc, 'http://schemas.xmlsoap.org/soap/envelope/', 'Body');
    envelope.appendChild(body);
    var parameterParent = body;
    parameterParent = createElementNS(xmlDoc, 'http://datespackage/', 'checkInteraction');
    body.appendChild(parameterParent);
    var paramEl = createElementNS(xmlDoc, 'http://datespackage/', 'ID');
    paramEl.appendChild(xmlDoc.createTextNode(_ID));
    parameterParent.appendChild(paramEl);
    var paramEl = createElementNS(xmlDoc, 'http://datespackage/', 'drugID');
    paramEl.appendChild(xmlDoc.createTextNode(_drugID));
    parameterParent.appendChild(paramEl);
    var resultsProcessor = function (responseDoc)
    {
        var resultsObj = null;
        body = firstChildElement(responseDoc.documentElement);
        if (localName(body) != 'Body')
        {
            body = nextSiblingElement(body);
        }
        
        var resultEl = firstChildElement(body);
        resultEl = firstChildElement(resultEl);
        resultObj = getText(resultEl);
        callback(resultObj);
    }
    invokeAsync('http://crius.dyndns.org:8888/JavaWebService-GetDates-context-root/GetDatesWSSoapHttpPort', xmlDoc, resultsProcessor);
}

function GetDatesWSSoapHttpPort_getRoute(_RFIDNum)
{
    var xmlDoc = createNewDocument();
    var envelope = createElementNS(xmlDoc, 'http://schemas.xmlsoap.org/soap/envelope/', 'Envelope');
    xmlDoc.appendChild(envelope);
    var body = createElementNS(xmlDoc, 'http://schemas.xmlsoap.org/soap/envelope/', 'Body');
    envelope.appendChild(body);
    var parameterParent = body;
    parameterParent = createElementNS(xmlDoc, 'http://datespackage/', 'getRoute');
    body.appendChild(parameterParent);
    var paramEl = createElementNS(xmlDoc, 'http://datespackage/', 'RFIDNum');
    paramEl.appendChild(xmlDoc.createTextNode(_RFIDNum));
    parameterParent.appendChild(paramEl);
    var responseDoc = invokeSync('http://crius.dyndns.org:8888/JavaWebService-GetDates-context-root/GetDatesWSSoapHttpPort', xmlDoc);
    var resultObj = null;
    body = firstChildElement(responseDoc.documentElement);
    if (localName(body) != 'Body')
    {
        body = nextSiblingElement(body);
    }
    
    var resultEl = firstChildElement(body);
    resultEl = firstChildElement(resultEl);
    resultObj = getText(resultEl);
    return resultObj;
}

function GetDatesWSSoapHttpPort_getRouteAsync(_RFIDNum, callback)
{
    var xmlDoc = createNewDocument();
    var envelope = createElementNS(xmlDoc, 'http://schemas.xmlsoap.org/soap/envelope/', 'Envelope');
    xmlDoc.appendChild(envelope);
    var body = createElementNS(xmlDoc, 'http://schemas.xmlsoap.org/soap/envelope/', 'Body');
    envelope.appendChild(body);
    var parameterParent = body;
    parameterParent = createElementNS(xmlDoc, 'http://datespackage/', 'getRoute');
    body.appendChild(parameterParent);
    var paramEl = createElementNS(xmlDoc, 'http://datespackage/', 'RFIDNum');
    paramEl.appendChild(xmlDoc.createTextNode(_RFIDNum));
    parameterParent.appendChild(paramEl);
    var resultsProcessor = function (responseDoc)
    {
        var resultsObj = null;
        body = firstChildElement(responseDoc.documentElement);
        if (localName(body) != 'Body')
        {
            body = nextSiblingElement(body);
        }
        
        var resultEl = firstChildElement(body);
        resultEl = firstChildElement(resultEl);
        resultObj = getText(resultEl);
        callback(resultObj);
    }
    invokeAsync('http://crius.dyndns.org:8888/JavaWebService-GetDates-context-root/GetDatesWSSoapHttpPort', xmlDoc, resultsProcessor);
}

function GetDatesWSSoapHttpPort_getSince(_timestamp)
{
    var xmlDoc = createNewDocument();
    var envelope = createElementNS(xmlDoc, 'http://schemas.xmlsoap.org/soap/envelope/', 'Envelope');
    xmlDoc.appendChild(envelope);
    var body = createElementNS(xmlDoc, 'http://schemas.xmlsoap.org/soap/envelope/', 'Body');
    envelope.appendChild(body);
    var parameterParent = body;
    parameterParent = createElementNS(xmlDoc, 'http://datespackage/', 'getSince');
    body.appendChild(parameterParent);
    var paramEl = createElementNS(xmlDoc, 'http://datespackage/', 'timestamp');
    paramEl.appendChild(xmlDoc.createTextNode(_timestamp));
    parameterParent.appendChild(paramEl);
    var responseDoc = invokeSync('http://192.168.10.20:8888/JavaWebService-GetDates-context-root/GetDatesWSSoapHttpPort', xmlDoc);
    var resultObj = null;
    body = firstChildElement(responseDoc.documentElement);
    if (localName(body) != 'Body')
    {
        body = nextSiblingElement(body);
    }
    
    var resultEl = firstChildElement(body);
    resultEl = firstChildElement(resultEl);
    resultObj = [];
    if (resultEl)
    {
        for (var achild = firstChildElement(resultEl.parentNode); achild; achild = nextSiblingElement(achild))
        {
            resultObj[resultObj.length] = getText(achild);
        }
    }
    return resultObj;
}

function GetDatesWSSoapHttpPort_getSinceAsync(_timestamp, callback)
{
    var xmlDoc = createNewDocument();
    var envelope = createElementNS(xmlDoc, 'http://schemas.xmlsoap.org/soap/envelope/', 'Envelope');
    xmlDoc.appendChild(envelope);
    var body = createElementNS(xmlDoc, 'http://schemas.xmlsoap.org/soap/envelope/', 'Body');
    envelope.appendChild(body);
    var parameterParent = body;
    parameterParent = createElementNS(xmlDoc, 'http://datespackage/', 'getSince');
    body.appendChild(parameterParent);
    var paramEl = createElementNS(xmlDoc, 'http://datespackage/', 'timestamp');
    paramEl.appendChild(xmlDoc.createTextNode(_timestamp));
    parameterParent.appendChild(paramEl);
    var resultsProcessor = function (responseDoc)
    {
        var resultsObj = null;
        body = firstChildElement(responseDoc.documentElement);
        if (localName(body) != 'Body')
        {
            body = nextSiblingElement(body);
        }
        
        var resultEl = firstChildElement(body);
        resultEl = firstChildElement(resultEl);
        resultObj = [];
        if (resultEl)
        {
            for (var achild = firstChildElement(resultEl.parentNode); achild; achild = nextSiblingElement(achild))
            {
                resultObj[resultObj.length] = getText(achild);
            }
        }
        callback(resultObj);
    }
    invokeAsync('http://crius.dyndns.org:8888/JavaWebService-GetDates-context-root/GetDatesWSSoapHttpPort', xmlDoc, resultsProcessor);
}

function GetDatesWSSoapHttpPort_deserialize_GetDates(valueEl)
{
    var resultsObject =
    {
    };
    for (var child = firstChildElement(valueEl); child; child = nextSiblingElement(child))
    {
        if (localName(child) == 'name')
        {
            resultsObject.name = getText(child);
        }
        if (localName(child) == 'desc')
        {
            resultsObject.desc = getText(child);
        }
    }
    return resultsObject;
}
