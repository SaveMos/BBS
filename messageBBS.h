#include <string>
#include <vector>
using namespace std;

class messageBBS{
private:
    uint32_t id;
    string author;
    string title;
    string body;

public:
     // Costruttore di default
    messageBBS() {}

    // Costruttore che inizializza tutti i membri della classe
    messageBBS(uint32_t newId, string newAuthor, string newTitle, string newBody){
        this->id = newId;
        this->author = newAuthor;
        this->title = newTitle;
        this->body = newBody;
    }

    messageBBS(int newId, string newAuthor, string newTitle, string newBody){
        this->id = static_cast<uint32_t>(newId);
        this->author = newAuthor;
        this->title = newTitle;
        this->body = newBody;
    }

    // Distruttore
    ~messageBBS() {
        // In questo caso, non è necessario fare nulla nel distruttore
    }

    void setId(uint32_t newId)
    {
        id = newId;
    }

    void setId(int newId)
    {
        id = static_cast<uint32_t>(newId);
    }

    void setAuthor(string newAuthor)
    {
        author = newAuthor;
    }

    void setTitle(string newTitle)
    {
        title = newTitle;
    }

    void setBody(const string &newBody)
    {
        body = newBody;
    }

    // Metodi get per ottenere i valori dei membri della classe
    uint32_t getId() const
    {
        return id;
    }

    string getAuthor() const
    {
        return author;
    }

    string getTitle() const
    {
        return title;
    }

    string getBody() const
    {
        return body;
    }

    void concatenateFields(string &ret)
    {
        ret = to_string(this->id) + "|" + this->author + "|" + this->title + "|" + this->body + "\0";
    }

    void deconcatenateFields(vector<string> &ret, string &input)
    {
        const char delimiter = '|';
        size_t pos = input.find(delimiter);   // Trova la posizione del primo carattere delimitatore

        while (pos != std::string::npos)
        {
            std::string parte = input.substr(0, pos); // Estrai la sottostringa prima del delimitatore 
            ret.push_back(parte); // Aggiungi la parte al vettore
            input = input.substr(pos + 1); // Rimuovi la parte estratta dalla stringa originale
            pos = input.find(delimiter);  // Trova la posizione del prossimo carattere delimitatore
        }
        ret.push_back(input); // Aggiungi l'ultima parte rimanente al vettore
    }

    void deconcatenateAndAssign(string &input){
        vector<string> fields;
        this->deconcatenateFields(fields , input);
        this->setId(stoi(fields.at(0)));
        this->setAuthor(fields[1]);
        this->setTitle(fields[2]);
        this->setBody(fields[3]);
    }

    void computeMAC(string &MAC)
    {
        MAC = "That MAC";
    }

    void encrypt(string &c)
    {
        c = "Cypher";
    }

    // Metodo per convertire l'oggetto in formato JSON
   string toJSON() const {
        string json = "{";
        json.append("\"id\": \"").append(to_string(this->id)).append("\",");
        json.append("\"author\": \"").append(author).append("\",");
        json.append("\"title\": \"").append(title).append("\",");
        json.append("\"body\": \"").append(body).append("\"");
        json.append("}");
        return json;
    }

    string toListed(){
        string ret = this->getAuthor() + " - " + this->getTitle() + "\n" + this->getBody() + "\n" + "\n";  
        return ret;
    }

    bool invalidMessage(){
        return this->getId() == 0;
    }
};
