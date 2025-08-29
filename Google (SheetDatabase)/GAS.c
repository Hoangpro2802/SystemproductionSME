function doGet(e) {
  var ss = SpreadsheetApp.getActiveSpreadsheet();
  var sheet = ss.getSheetByName("ID");
  var data = sheet.getDataRange().getValues();

  var vID = e && e.parameter && e.parameter.vID ? e.parameter.vID : null;
  var result = {};

  for (var i = 1; i < data.length; i++) {
    if (String(data[i][0]) === String(vID)) {
      result = {
        UID: data[i][0],
        name: data[i][1],
        lapRap: data[i][2],
        dongGoi: data[i][3],
        suaChua: data[i][4],
        checkIn: data[i][5],
        checkOut: data[i][6],
        completed: data[i][7],
        failed: data[i][8],
        lastUpdate: data[i][9]
      };
      break;
    }
  }

  return ContentService.createTextOutput(JSON.stringify(result, null, 2))
    .setMimeType(ContentService.MimeType.JSON);
}

function doPost(e) {
  var sheet = SpreadsheetApp.getActiveSpreadsheet().getSheetByName("ID");
  var data = sheet.getDataRange().getValues();
  var params = JSON.parse(e.postData.contents);

  for (var i = 1; i < data.length; i++) {
    if (String(data[i][0]) === String(params.vID)) {
      var row = i + 1;

      if (params.type === "checkin") {
        sheet.getRange(row, 6).setValue(new Date());
      } else if (params.type === "checkout") {
        sheet.getRange(row, 7).setValue(new Date());
        sheet.getRange(row, 😎.setValue(params.completed);
        sheet.getRange(row, 9).setValue(params.failed);
      }

      sheet.getRange(row, 10).setValue(new Date());
      break;
    }
  }

  return ContentService.createTextOutput(JSON.stringify({status: "success"}))
    .setMimeType(ContentService.MimeType.JSON);
}