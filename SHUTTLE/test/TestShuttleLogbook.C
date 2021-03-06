
TSQLServer *fServer=0;

//______________________________________________________________________________________________
Bool_t Connect(){
// Connect to MySQL Server of the DAQ logbook

	// check connection: if already connected return
	if(fServer && fServer->IsConnected()) return kTRUE;

//	TString serverHost="mysql://pcald30.cern.ch";
	TString serverHost="mysql://aldaqgw01-gpn.cern.ch:1434";
	fServer = TSQLServer::Connect(serverHost.Data(),"shuttle","alice");
	
	printf("Connecting to mysql server on %s !!!!!!\n", serverHost.Data());

	if (!fServer || !fServer->IsConnected()) {
		printf("Can't establish connection to DAQ log book DB!\n");
		if(fServer) delete fServer;
		return kFALSE;
	}

	// Get table
	TSQLResult* aResult=0;
	aResult = fServer->GetTables("SHUTTLE_TEST");
	delete aResult;
	return kTRUE;
}

//______________________________________________________________________________________________
Bool_t QueryShuttleLogbook(AliShuttleLogbookEntry& entry, Int_t runNumber=-1)
{
// Query DAQ's Shuttle logbook and fills detector status array

	if(runNumber<=0 && entry.GetRun()<=0) {
		printf("Use a valid Run number!\n");
		return kFALSE;
	}
	if(!Connect()) return kFALSE;
	if(runNumber<=0) runNumber= entry.GetRun();
	//entry.SetRun(runNumber); 

	// check connection, in case connect
	if(!Connect()) return kFALSE;

	TString sqlQuery;
	sqlQuery = Form("select * from logbook_shuttle where run = %d", runNumber);

	TSQLResult* aResult = fServer->Query(sqlQuery);
	if (!aResult) {
		printf("Can't execute query <%s>!\n", sqlQuery.Data());
		return kFALSE;
	}

	// TODO Check field count!
//	if (aResult->GetFieldCount() != 24) {
//		printf("Invalid SQL result field number!\n");
//		delete aResult;
//		return kFALSE;
//	}

	TSQLRow* aRow;
	while ((aRow = aResult->Next())) {
		TString runString(aRow->GetField(0), aRow->GetFieldLength(0));
		Int_t run = runString.Atoi();

		// loop on detectors
		for(UInt_t ii = 0; ii < 22; ii++){
			entry.SetDetectorStatus(aResult->GetFieldName(ii), aRow->GetField(ii));
		}

		delete aRow;
	}

	delete aResult; aResult=0;

	// Query run parameters from logbook!

	sqlQuery = Form("select * from logbook where run=%d", runNumber);

	aResult = fServer->Query(sqlQuery);
	if (!aResult) {
		printf("Can't execute query <%s>!", sqlQuery.Data());
		return kFALSE;
	}

	if(aResult->GetRowCount() == 0) {
		printf("QueryRunParameters - No entry in DAQ Logbook for run %d!", runNumber);
		delete aResult;
		return kFALSE;
	}

	if(aResult->GetRowCount() > 1) {
		printf("More than one entry in DAQ Logbook for run %d!", runNumber);
		delete aResult;
		return kFALSE;
	}

	while ((aRow = aResult->Next())) {

		for(UInt_t ii = 0; ii < aResult->GetFieldCount(); ii++)
			entry.SetRunParameter(aResult->GetFieldName(ii), aRow->GetField(ii));

		UInt_t startTime, endTime;
		TString startTimeString = entry.GetRunParameter("time_start");
		UInt_t startTime = startTimeString.Atoi();
		TString endTimeString = entry.GetRunParameter("time_end");
		UInt_t endTime = endTimeString.Atoi();

		if (!startTime || !endTime || startTime > endTime) {
			printf("QueryRunParameters - Invalid parameters for Run %d: startTime = %d, endTime = %d",
					runNumber, startTime, endTime);
			delete aRow;
			delete aResult;
			return kFALSE;
		}

//		entry.SetStartTime(startTime);
//		entry.SetEndTime(endTime);

		delete aRow;
	}

	entry.Print("all");

	delete aResult;
	return kTRUE;
}
//______________________________________________________________________________________________
Bool_t UpdateShuttleLogbook(AliShuttleLogbookEntry& entry, Int_t runNumber=-1)
{
  // Update Shuttle logbook table - TEST ONLY, USE WITH CARE!


	if(runNumber<=0 && entry.GetRun()<=0) {
		printf("Use a valid Run number!\n");
		return kFALSE;
	}
	if(!Connect()) return kFALSE;
	if(runNumber<=0) runNumber= entry.GetRun();
	entry.SetRun(runNumber); 

	TString sqlQuery("update logbook_shuttle set ");

	for(UInt_t i=0; i < AliShuttleInterface::NDetectors(); i++){
		sqlQuery += Form("%s=\"%s\"", AliShuttleInterface::GetDetName(i), entry.GetDetectorStatusName(entry.GetDetectorStatus(i)));
		if(i < AliShuttleInterface::NDetectors()-1) sqlQuery += ", ";
	}

	sqlQuery += Form(" where run=%d;",entry.GetRun());

	printf("sqlQuery: %s\n", sqlQuery.Data());

	TSQLResult* aResult;
	aResult = fServer->Query(sqlQuery);
	if (!aResult) {
		printf("Can't execute query <%s>!\n", sqlQuery.Data());
		return kFALSE;
	}

	delete aResult;

	return kTRUE;
}

//______________________________________________________________________________________________
Bool_t UpdateShuttleLogbook(Int_t runNumber, const char* detCode, AliShuttleLogbookEntry::Status status)
{
  // Update Shuttle logbook table - TEST ONLY, USE WITH CARE!


	if(AliShuttleInterface::GetDetPos(detCode) < 0) return kFALSE;
	if(!Connect()) return kFALSE;

	TString sqlQuery("update logbook_shuttle set ");


	sqlQuery += Form("%s=\"%s\" ", detCode, AliShuttleLogbookEntry::GetDetectorStatusName(status));

	sqlQuery += Form("where run=%d;",runNumber);

	printf("sqlQuery: %s\n", sqlQuery.Data());

	TSQLResult* aResult;
	aResult = fServer->Query(sqlQuery);
	if (!aResult) {
		printf("Can't execute query <%s>!\n", sqlQuery.Data());
		return kFALSE;
	}

	delete aResult;

	return kTRUE;
}

//______________________________________________________________________________________________
Bool_t UpdateShuttleLogbook(Int_t runNumber, const char* detCode, const char* status)
{
  // Update Shuttle logbook table - TEST ONLY, USE WITH CARE!


	if(AliShuttleInterface::GetDetPos(detCode) < 0) return kFALSE;
	if(!Connect()) return kFALSE;

	TString sqlQuery("update logbook_shuttle set ");


	sqlQuery += Form("%s=\"%s\" ", detCode, status);

	sqlQuery += Form("where run=%d;",runNumber);

	printf("sqlQuery: %s\n", sqlQuery.Data());

	TSQLResult* aResult;
	aResult = fServer->Query(sqlQuery);
	if (!aResult) {
		printf("Can't execute query <%s>!\n", sqlQuery.Data());
		return kFALSE;
	}

	delete aResult;

	return kTRUE;
}

//______________________________________________________________________________________________
Bool_t InsertNewRun(AliShuttleLogbookEntry& entry, Int_t runNumber=-1)
{
  // Update Shuttle logbook table - TEST ONLY, USE WITH CARE!

	if(runNumber<=0 && entry.GetRun()<=0) {
		printf("Use a valid Run number!\n");
		return kFALSE;
	}
	if(!Connect()) return kFALSE;
	if(runNumber<=0) runNumber= entry.GetRun();
	entry.SetRun(runNumber); 

	TString sqlQuery = Form("insert into logbook_shuttle (run) values (%d);", runNumber);

	printf("sqlQuery: %s\n", sqlQuery.Data());

	TSQLResult* aResult;
	aResult = fServer->Query(sqlQuery);
	if (!aResult) {
		printf("Can't execute query <%s>!\n", sqlQuery.Data());
		return kFALSE;
	}

	delete aResult;

	UpdateShuttleLogbook(entry);

	return kTRUE;
}

//______________________________________________________________________________________________
Bool_t DeleteEntries(Int_t runNumber=-1)
{
  // Update Shuttle logbook table - TEST ONLY, USE WITH CARE!

	if(!Connect()) return kFALSE;
	
	TString runStr;
	if(runNumber>0) runStr=Form("where run=%d",runNumber);
	TString sqlQuery = Form("delete from logbook_shuttle %s;", runStr.Data());

	printf("sqlQuery: %s\n", sqlQuery.Data());

	TSQLResult* aResult;
	aResult = fServer->Query(sqlQuery);
	if (!aResult) {
		printf("Can't execute query <%s>!\n", sqlQuery.Data());
		return kFALSE;
	}

	delete aResult;

	return kTRUE;
}

//______________________________________________________________________________________________
void TestShuttleLogbook(){

	gSystem->Load("libSHUTTLE");
	
	DeleteEntries();

	AliShuttleLogbookEntry::Status y[18]={1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};
	AliShuttleLogbookEntry lb(21242, y);
	lb.SetDetectorStatus("HMP","Unprocessed"); // HMPID
	lb.SetDetectorStatus("ZDC","Unprocessed"); // ZDC
	lb.SetDetectorStatus("TPC","Unprocessed"); // TPC
	lb.SetDetectorStatus("SPD","Unprocessed"); // SPD
	lb.SetDetectorStatus("GRP","Unprocessed"); // GRP
	lb.Print();

	InsertNewRun(lb);
	InsertNewRun(lb,21243);
	InsertNewRun(lb,21244);
	InsertNewRun(lb,21245);
//	InsertNewRun(lb,21246);
//	InsertNewRun(lb,1000);

}
