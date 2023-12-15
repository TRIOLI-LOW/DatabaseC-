#include <iostream>
#include <pqxx/pqxx>
#include <Windows.h>


class DatabaseClients {
public:
	DatabaseClients(const std::string& conn);
	//~DatabaseClients();
	
	void createTable();
	void addClient(const std::string& name, const std::string last_name, const std::string email);
	void addPhone(int client_id, const std::string& phones );
	void updateClient(int client_id, const std::string& name, const std::string last_name, const std::string email);
	void removePhone(int client_id, const std::string& phones);
	void removeClient(int client_id);
	void findClient(const std::string& find);

private:
	pqxx::connection* conn;
};


DatabaseClients::DatabaseClients(const std::string& connectionString) {
	conn = new pqxx::connection(connectionString);
}

void DatabaseClients::createTable() {
	pqxx::work tx(*conn);
	tx.exec("CREATE TABLE IF NOT EXISTS clients (id SERIAL PRIMARY KEY, name TEXT, last_name TEXT, email TEXT);");
	tx.exec("CREATE TABLE IF NOT EXISTS phones (id SERIAL PRIMARY KEY, client_id INT, phone_number TEXT);");
	tx.commit();
};
void DatabaseClients::addClient(const std::string& name, const std::string last_name, const std::string email) {
	pqxx::work tx(*conn);
	tx.exec_params("INSERT INTO clients (name, last_name, email) VALUES ($1, $2, $3) RETURNING id;", name, last_name, email);
	tx.commit();
}

void DatabaseClients::addPhone(int client_id, const std::string& phone_number) {
	pqxx::work tx(*conn);
	pqxx:: result res = tx.exec_params("INSERT INTO phones (client_id, phone_number) VALUES ($1, $2) RETURNING id;", client_id, phone_number);
	tx.commit();
}
void DatabaseClients::updateClient(int client_id, const std::string& name, const std::string last_name, const std::string email) {
	pqxx::work tx(*conn);
	tx.exec_params("UPDATE clients SET name = $1, last_name = $2, email = $3 WHERE id = $4;", name, last_name, email, client_id );
	tx.commit();
};
void DatabaseClients::removePhone(int client_id, const std::string& phone_number) {
	pqxx::work tx(*conn);
	tx.exec_params("DELETE FROM phones WHERE client_id = $1 AND phone_number = $2;", client_id, phone_number);
	tx.commit();
};
void DatabaseClients::removeClient(int client_id) {
	pqxx::work tx(*conn);
	tx.exec_params("DELETE FROM clients WHERE id = $1;", client_id);
	tx.commit();
};

void DatabaseClients::findClient(const std::string& find) {
	pqxx::work tx(*conn);
	pqxx::result result = tx.exec_params(
        "SELECT c.*, p.phone_number FROM clients c "
        "LEFT JOIN phones p ON c.id = p.client_id "
		"WHERE c.name = $1 OR c.last_name = $1 OR c.email = $1 OR p.phone_number = $1;", find);
	
	std::cout << "Ðåçóëüòàò: " << std::endl;
	for (const auto res : result) {
		int id = res[0].as<int>();
		std::string name = res[1].as<std::string>();
		std::string last_name = res[2].as<std::string>();
		std::string email = res[3].as<std::string>();
		std::string phones = res[4].is_null() ? "" : res[4].as <std::string>();

		std::cout << "ID: " << id << std::endl;
		std::cout << "ÈÌß: " << name << std::endl;
		std::cout << "ÔÀÌÈËÈß: " << last_name << std::endl;
		std::cout << "Email: " << email << std::endl;
		std::cout << "Íîìåð òåëåôîíà: " << phones << std::endl;
	}
	tx.commit();
};

int main() {
	
	SetConsoleCP(CP_UTF8);
	SetConsoleOutputCP(CP_UTF8);
	try {

		DatabaseClients db("host=127.0.0.1 "
			"port=5432 "
			"dbname=homedb "
			"user=postgres "
			"password=911215171");
		std::cout << "hello!" << std::endl;
		db.createTable();
		db.addPhone(2, "89335556069");
		db.addPhone(2, "88009994045");
		db.addClient("Dima", "Pestov", "pst@gmail.com");
		db.removePhone(1, "89335556069");
		db.removeClient(4);
		db.updateClient(2, "Egor", "Megrin", "fasomy@gmail.com");
		db.findClient("Dima");
		db.findClient("Egor");
	}
	catch (const std::exception& e) {
		std::cout << e.what() << std::endl;
	}


}
