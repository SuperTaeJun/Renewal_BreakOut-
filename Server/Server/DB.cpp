#include "pch.h"
#include "DB.h"

#define NAME_LEN 20 


void show_err() {
	cout << "error" << endl;
}

void printSQLError(SQLHANDLE handle, SQLSMALLINT type) {
	SQLWCHAR sqlState[1024];
	SQLWCHAR message[1024];
	SQLINTEGER nativeError;
	SQLSMALLINT msgLength;
	SQLRETURN ret;
	SQLSMALLINT i = 1;
	while ((ret = SQLGetDiagRec(type, handle, i, sqlState, &nativeError, message, 1024, &msgLength)) != SQL_NO_DATA) {
		std::wcerr << L"SQL error: " << message << L", SQL state: " << sqlState << L", Native error: " << nativeError << std::endl;
		i++;
	}
}

void save_data(const char* id, const char* pw) {
	SQLHENV henv;
	SQLHDBC hdbc;
	SQLHSTMT hstmt = 0;
	SQLRETURN retcode;


	wchar_t temp[BUFSIZE] = {};
	swprintf_s(temp, BUFSIZE, L"EXEC [GraduationWork].[dbo].create_user_data N'%S', N'%S'", id, pw);

	// ȯ�� �ڵ� �Ҵ�
	retcode = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &henv);

	// ODBC ���� ����
	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
		retcode = SQLSetEnvAttr(henv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER*)SQL_OV_ODBC3, 0);

		// ���� �ڵ� �Ҵ�
		if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
			retcode = SQLAllocHandle(SQL_HANDLE_DBC, henv, &hdbc);

			// �α��� Ÿ�Ӿƿ� ���� (5��)
			if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
				SQLSetConnectAttr(hdbc, SQL_LOGIN_TIMEOUT, (SQLPOINTER)5, 0);

				// ������ �ҽ��� ����
				retcode = SQLConnect(hdbc, (SQLWCHAR*)L"2024GraduationWork", SQL_NTS, (SQLWCHAR*)NULL, 0, NULL, 0);

				// ���� �ڵ� �Ҵ�
				if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
					std::wcout << L"ODBC Connection Success" << std::endl;
					retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
					retcode = SQLExecDirect(hstmt, (SQLWCHAR*)temp, SQL_NTS);
					if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
						std::wcout << L"Data inserted successfully" << std::endl;
					}
					else {
						std::wcerr << L"Error executing SQL statement" << std::endl;
						printSQLError(hstmt, SQL_HANDLE_STMT);
					}

					// ���� �ڵ� ����
					SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
				}
				else {
					std::wcerr << L"ODBC Connection Failed" << std::endl;
					printSQLError(hdbc, SQL_HANDLE_DBC);
				}

				// ���� ����
				SQLDisconnect(hdbc);
			}
			SQLFreeHandle(SQL_HANDLE_DBC, hdbc);
		}
		SQLFreeHandle(SQL_HANDLE_ENV, henv);
	}
}


bool DB_odbc(const char* name, const char* pw)
{
	SQLHENV henv;
	SQLHDBC hdbc;
	SQLHSTMT hstmt = 0;
	SQLRETURN retcode;

	char temp[BUFSIZE] = {};
	sprintf_s(temp, sizeof(temp), "EXEC [GraduationWork].[dbo].login_player %s,%s", name, pw);
	wchar_t* exec;
	int str_size = MultiByteToWideChar(CP_ACP, 0, temp, -1, NULL, NULL);
	exec = new WCHAR[str_size];
	MultiByteToWideChar(CP_ACP, 0, temp, sizeof(temp) + 1, exec, str_size);

	SQLWCHAR p_id[NAME_LEN] = {};
	SQLLEN cbP_ID = 0;

	retcode = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &henv);

	// Set the ODBC version environment attribute  
	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
		retcode = SQLSetEnvAttr(henv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER*)SQL_OV_ODBC3, 0);


		if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
			retcode = SQLAllocHandle(SQL_HANDLE_DBC, henv, &hdbc);


			if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
				SQLSetConnectAttr(hdbc, SQL_LOGIN_TIMEOUT, (SQLPOINTER)5, 0);


				retcode = SQLConnect(hdbc, (SQLWCHAR*)L"2024GraduationWork", SQL_NTS, (SQLWCHAR*)NULL, 0, NULL, 0);


				if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
					cout << "ODBC Connection Success" << endl;
					retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
					retcode = SQLExecDirect(hstmt, (SQLWCHAR*)exec, SQL_NTS);
					if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
						retcode = SQLBindCol(hstmt, 1, SQL_C_WCHAR, p_id, NAME_LEN, &cbP_ID);

						for (int i = 0; ; i++) {
							retcode = SQLFetch(hstmt);
							if (retcode == SQL_ERROR || retcode == SQL_SUCCESS_WITH_INFO)
								show_err();
							if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
							{
								if (p_id != 0) {
									return true;
								}
								else
								{
									cout << "login Failed" << endl;
									return false;
								}
							}
							else
								break;
						}
					}
					if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
						SQLCancel(hstmt);
						SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
					}

					SQLDisconnect(hdbc);
				}
				else cout << "ODBC Connected Failed" << endl;
				SQLFreeHandle(SQL_HANDLE_DBC, hdbc);
			}
		}
		SQLFreeHandle(SQL_HANDLE_ENV, henv);
	}

	return false;
}

//�÷��̾� ���̵� Ȯ��
bool DB_id(const char* login_id)
{
	SQLHENV henv;
	SQLHDBC hdbc;
	SQLHSTMT hstmt = 0;
	SQLRETURN retcode;
	

	char temp[BUFSIZE] = {};
	sprintf_s(temp, sizeof(temp), "EXEC [GraduationWork].[dbo].check_player_id %s", login_id);
	wchar_t* exec;
	int str_size = MultiByteToWideChar(CP_ACP, 0, temp, -1, NULL, NULL);
	exec = new WCHAR[str_size];
	MultiByteToWideChar(CP_ACP, 0, temp, sizeof(temp) + 1, exec, str_size);

	SQLWCHAR p_id[NAME_LEN] = {};
	SQLLEN cbP_ID = 0;

	retcode = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &henv);

	// Set the ODBC version environment attribute  
	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
		retcode = SQLSetEnvAttr(henv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER*)SQL_OV_ODBC3, 0);


		if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
			retcode = SQLAllocHandle(SQL_HANDLE_DBC, henv, &hdbc);


			if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
				SQLSetConnectAttr(hdbc, SQL_LOGIN_TIMEOUT, (SQLPOINTER)5, 0);


				retcode = SQLConnect(hdbc, (SQLWCHAR*)L"2024GraduationWork", SQL_NTS, (SQLWCHAR*)NULL, 0, NULL, 0);


				if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
					cout << "ODBC Connection Success" << endl;
					retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
					retcode = SQLExecDirect(hstmt, (SQLWCHAR*)exec, SQL_NTS);
					if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
						retcode = SQLBindCol(hstmt, 1, SQL_C_WCHAR, p_id, NAME_LEN, &cbP_ID);

						for (int i = 0; ; i++) {
							retcode = SQLFetch(hstmt);
							if (retcode == SQL_ERROR || retcode == SQL_SUCCESS_WITH_INFO)
								show_err();
							if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
							{
								if (p_id != 0) {
									cout << "[DB_ID] ���̵� ���� "<< endl;
									return true;
								}
								else
								{
									cout << " [DB_ID] ���� " << endl;
									return false;
								}
							}
							else
								break;
						}
					}

					// Process data  
					if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
						SQLCancel(hstmt);
						SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
					}

					SQLDisconnect(hdbc);
				}
				else cout << "ODBC Connected Failed" << endl;
				SQLFreeHandle(SQL_HANDLE_DBC, hdbc);
			}
		}
		SQLFreeHandle(SQL_HANDLE_ENV, henv);
	}

	cout << " [DB_ID] ����" << endl;
	return false;
}
