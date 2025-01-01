const sheetId = "1pVsE6PkV9vPfiwpWD9ZNg6BJUgZ5na8kN9TnuO4kAPw"; // Replace with your Google Sheet ID
const sheetName = "Rainfall-Flood"; // Replace with your desired sheet name

function doGet(e) {
  const sheet = SpreadsheetApp.openById(sheetId).getSheetByName(sheetName);

  // Handle Rain Gauge Data
  if (e.parameter.tips || e.parameter.rain) {
    const tips = e.parameter.tips || "0";
    const rain = e.parameter.rain || "0.0";
    const date = new Date();

    sheet.appendRow(["Rain Gauge", date, "", "", tips, rain, "", ""]);
    return ContentService.createTextOutput("Rain Gauge Data Added Successfully");
  }

  // Handle Upstream Data
  if (e.parameter.upstream) {
    const upstream = e.parameter.upstream || "";
    const statusSwitchUS = e.parameter.statusSwitchUS || "";
    const date = new Date();
    const formattedDate = date.toLocaleDateString('en-GB', { day: '2-digit', month: '2-digit', year: '2-digit' });

    sheet.appendRow(["Upstream", formattedDate, statusSwitchUS, upstream]);
    return ContentService.createTextOutput("Upstream Data Added Successfully");
  }

  return ContentService.createTextOutput("No Valid Parameters Provided");
}

function doPost(e) {
  const sheet = SpreadsheetApp.openById(sheetId).getSheetByName(sheetName);

  // Handle Rain Gauge Data
  if (e.parameter.tips || e.parameter.rain) {
    const tips = e.parameter.tips || "0";
    const rain = e.parameter.rain || "0.0";
    const date = new Date();

    sheet.appendRow(["Rain Gauge", date, "", "", tips, rain, "", ""]);
    return ContentService.createTextOutput("Rain Gauge Data Added Successfully");
  }

  // Handle Upstream Data
  if (e.parameter.upstream) {
    const upstream = e.parameter.upstream || "";
    const statusSwitchUS = e.parameter.statusSwitchUS || "";
    const date = new Date();
    const formattedDate = date.toLocaleDateString('en-GB', { day: '2-digit', month: '2-digit', year: '2-digit' });

    sheet.appendRow(["Upstream", formattedDate, statusSwitchUS, upstream]);
    return ContentService.createTextOutput("Upstream Data Added Successfully");
  }

  return ContentService.createTextOutput("No Valid Parameters Provided");
}
