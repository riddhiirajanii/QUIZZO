# QUIZZO - Online Quiz System

**QUIZZO** is a simple, offline-compatible online quiz system built using HTML, CSS, JavaScript (frontend), C with CGI for backend processing, and Python for advanced search functionality. It runs locally using **XAMPP** for Apache hosting and CGI support.

---

## 📁 Folder Structure
QUIZZO/
├── cgi-bin/            <-- Compiled CGI executables (.cgi)
├── www/                <-- ✅ All QUIZZO frontend files (HTML/CSS/JS)
├── quiz_data.json      <-- JSON file for quiz search
├── search_server.py    <-- Python Flask search backend
├── README.md           <-- Setup + usage instructions


---

## ⚙️ Requirements

- **XAMPP** (Apache with CGI enabled)
- **Python 3.x**
  - Install `Flask` and `flask-cors`:
    ```bash
    pip install flask flask-cors
    ```

---

## 🚀 How to Run

###  1. Set up XAMPP for CGI
- Place `signup.exe`, `login.exe`, and other CGI files in `C:/xampp/cgi-bin/`
- Ensure `httpd.conf` contains:
  ```apache
  ScriptAlias /cgi-bin/ "C:/xampp/cgi-bin/"
  <Directory "C:/xampp/cgi-bin">
      AllowOverride None
      Options +ExecCGI
      Require all granted
      AddHandler cgi-script .cgi .exe
  </Directory>

### 2. Run the python search server
 ```bash
    cd path/to/QUIZZO
    python search_server.py
    ```


🧠 Features
1. 🔐 Signup and login using CGI (C backend)

2. 🧑 User dashboard with:

3. Profile (username, rank, score)

4. Leaderboard (dynamically updated)

5. Quizzes (clickable links)

6. 🔎 Live search bar for quizzes (tag-based search via Python Flask)

7. ✨ Ambient UI with scrollable dropdowns and filters

📌 Notes
1. All user data is stored in users.txt

2. Rating and rank updates are calculated based on quiz performance

3. Search supports partial title or tag matching

