from flask import Flask, render_template, request, redirect, url_for, session, flash, url_for
import psycopg2
from uuid import uuid4

app = Flask(__name__)

app.secret_key = "teste"

def connect():
    conn = psycopg2.connect(database="projeto",
                            user="iot",
                            password="pepcon-garton",
                            host="postgresql.janks.dev.br",
                            port="5432")
    return conn


@app.route('/', methods=['GET', 'POST'])
@app.route('/login', methods=['GET', 'POST'])
def login():
    
    if request.method == 'POST' and 'username' in request.form and 'password' in request.form:
        conn = connect()
        cursor = conn.cursor()

        username = request.form['username']
        password = request.form['password']

        # 1) Tenta logar como ADMIN
        cursor.execute('SELECT * FROM admin_cafeteria WHERE username = %s AND password = %s', (username, password))
        admin = cursor.fetchone()
        if admin:
            session['loggedin'] = True
            session['id'] = admin[0]
            session['username'] = admin[1]
            session['role'] = 'admin'
            cursor.close()
            conn.close()
            return redirect('/home')
        
        # 2) Se não for admin, tenta logar como USUÁRIO
        cursor.execute(
            'SELECT * FROM users_cafeteria WHERE name = %s AND rfid = %s',
            (username, password)
        )
        user = cursor.fetchone()

        cursor.close()
        conn.close()

        if user:
            session['loggedin'] = True
            session['id'] = user[0]
            session['username'] = user[1]
            session['rfid'] = user[3]
            session['role'] = 'user'
            id=user[0]
            return redirect(url_for('add_balance', id=id))

    return render_template('login.html')

@app.route('/logout')
def logout():
    session.pop('loggedin', None)
    session.pop('id', None)
    session.pop('username', None)
    return redirect(url_for('login'))

@app.route('/home')
def home():
    if 'loggedin' not in session or session['role'] != "admin":
        return redirect('/login')
    return render_template('home.html', username=session["username"])

#       ///////                                 admin                   //////

@app.route('/list_adm')
def list_adm():
    if 'loggedin' not in session or session['role'] != "admin":
        return redirect('/login')
    conn = connect()
    cursor = conn.cursor()

    cursor.execute('SELECT id, username FROM admin_cafeteria')
    adms = cursor.fetchall()
    
    cursor.close()
    conn.close()
    
    return render_template("list_adm.html", adms=adms)

@app.route('/register_adm', methods=['GET','POST'])
def register_adm():
    if 'loggedin' not in session or session['role'] != "admin":
        return redirect('/login')
    
    if request.method == 'POST' and 'username' in request.form and 'password' in request.form:
        conn = connect()
        cursor = conn.cursor()
        
        cursor.execute('SELECT MAX(id) FROM admin_cafeteria')
        id = cursor.fetchone()[0] + 1
        username = request.form['username']
        password = request.form['password']
        cursor.execute("INSERT INTO admin_cafeteria (id, username, password) VALUES (%s, %s, %s)", (str(id), username, password))
        
        conn.commit()
        
        cursor.close()
        conn.close()
        return redirect('/list_adm')

    return render_template('register_adm.html')

@app.route('/confirm_delete_adm/<id>')
def confirm_delete_adm(id):
    if 'loggedin' not in session or session['role'] != "admin":
        return redirect('/login')
        

    conn = connect()
    cursor = conn.cursor()

    cursor.execute('SELECT id, username FROM admin_cafeteria WHERE id=%s',(id))
    adm = cursor.fetchone()
    
    cursor.close()
    conn.close()

    return render_template("confirm_delete_adm.html", username=adm[1], id=adm[0])

@app.route('/delete_adm/<id>')
def delete_adm(id):
    if 'loggedin' not in session or session['role'] != "admin":
        return redirect('/login')
    
    conn = connect()
    cursor = conn.cursor()

    cursor.execute('DELETE FROM admin_cafeteria WHERE id=%s', (id))
    
    conn.commit()
    
    cursor.close()
    conn.close()
    
    return redirect('/list_adm')

@app.route('/update_adm/<id>', methods = ['GET','POST'])
def update_adm(id):
    if 'loggedin' not in session or session['role'] != "admin":
        return redirect('/login')

    conn = connect()
    cursor = conn.cursor()
    
    cursor.execute('SELECT * FROM admin_cafeteria WHERE id=%s', (id))
    adm = cursor.fetchone()
    
    username = adm[1]
    senha = adm[2]
    
    if request.method == 'POST' and 'username' in request.form and 'password' in request.form:

        username = request.form['username']
        senha = request.form['password']
        
        cursor.execute('UPDATE admin_cafeteria SET username=%s, password=%s WHERE id=%s', (username, senha, id))

        conn.commit()
        
        cursor.close()
        conn.close()
        return redirect('/list_adm')

    cursor.close()
    conn.close()

    return render_template('update_adm.html', id=id, username=username, password=senha)


#       ///////                                 usuario                   //////

@app.route('/list_user')
def list_user():
    if 'loggedin' not in session or session['role'] != "admin":
        return redirect('/login')
    conn = connect()
    cursor = conn.cursor()

    cursor.execute('SELECT id, name FROM users_cafeteria')
    users = cursor.fetchall()
    
    cursor.close()
    conn.close()
    
    return render_template("list_user.html", users=users)

@app.route('/register_user', methods=['GET','POST'])
def register_user():
    if 'loggedin' not in session or session['role'] != "admin":
        return redirect('/login')
    
    if request.method == 'POST' and 'name' in request.form and 'rfid' in request.form and 'balance' in request.form:
        conn = connect()
        cursor = conn.cursor()
        
        cursor.execute('SELECT MAX(id) FROM users_cafeteria')
        id = cursor.fetchone()[0] + 1
        name = request.form['name']
        rfid = request.form['rfid']
        balance = request.form['balance']
        if rfid == '':
            rfid = None
        cursor.execute("INSERT INTO users_cafeteria (id, name, rfid, balance) VALUES (%s, %s, %s, %s )", (str(id), name, rfid, balance))
        
        conn.commit()
        
        cursor.close()
        conn.close()
        return redirect('/list_user')

    return render_template('register_user.html')

@app.route('/update_user/<id>', methods = ['GET','POST'])
def update_user(id):
    if 'loggedin' not in session or session['role'] != "admin":
        return redirect('/login')

    conn = connect()
    cursor = conn.cursor()
    
    cursor.execute('SELECT * FROM users_cafeteria WHERE id=%s', (id))
    user = cursor.fetchone()
    
    nome = user[1]
    rfid = user[2]
    if rfid == None:
        rfid = ''
    balance = user[3]
    
    if request.method == 'POST' and 'name' in request.form and 'rfid' in request.form:

        nome = request.form['name']
        rfid = request.form['rfid']
        balance = request.form['balance']

        
        cursor.execute('UPDATE users_cafeteria SET name=%s, rfid=%s, balance=%s WHERE id=%s', (nome, rfid, balance, id))

        conn.commit()
        
        cursor.close()
        conn.close()
        return redirect('/list_user')

    cursor.close()
    conn.close()

    return render_template('update_user.html', id=id, name=nome, rfid=rfid, balance=balance)

@app.route('/confirm_delete_user/<id>')
def confirm_delete_user(id):
    if 'loggedin' not in session or session['role'] != "admin":
        return redirect('/login')
        

    conn = connect()
    cursor = conn.cursor()

    cursor.execute('SELECT id, name FROM users_cafeteria WHERE id=%s',(id))
    user = cursor.fetchone()
    
    cursor.close()
    conn.close()

    return render_template("confirm_delete_user.html", name=user[1], id=user[0])

@app.route('/delete_user/<id>')
def delete_user(id):
    if 'loggedin' not in session or session['role'] != "admin":
        return redirect('/login')
    
    conn = connect()
    cursor = conn.cursor()

    cursor.execute('DELETE FROM users_cafeteria WHERE id=%s', (id))
    
    conn.commit()
    
    cursor.close()
    conn.close()
    
    return redirect('/list_user')

#       ///////                                 produto                   //////

@app.route('/list_prod')
def list_prod():
    if 'loggedin' not in session or session['role'] != "admin":
        return redirect('/login')
    conn = connect()
    cursor = conn.cursor()

    cursor.execute('SELECT id, nome FROM produtos_cafeteria')
    products = cursor.fetchall()
    
    cursor.close()
    conn.close()
    
    return render_template("list_prod.html", products=products)

@app.route('/register_prod', methods=['GET','POST'])
def register_prod():
    if 'loggedin' not in session or session['role'] != "admin":
        return redirect('/login')
    
    if request.method == 'POST' and 'nome' in request.form and 'preco_100g' in request.form and 'maquina' in request.form:
        conn = connect()
        cursor = conn.cursor()
        
        cursor.execute('SELECT MAX(id) FROM produtos_cafeteria')
        id = cursor.fetchone()[0] + 1
        nome = request.form['nome']
        preco_100g = request.form['preco_100g']
        maquina = request.form['maquina']
        cursor.execute("INSERT INTO produtos_cafeteria (id, nome, preco_100g, tipo_maquina) VALUES (%s, %s, %s, %s )", (str(id), nome, preco_100g, maquina))
        
        conn.commit()
        
        cursor.close()
        conn.close()
        return redirect('/list_prod')

    return render_template('register_prod.html')

@app.route('/update_prod/<id>', methods = ['GET','POST'])
def update_prod(id):
    if 'loggedin' not in session or session['role'] != "admin":
        return redirect('/login')

    conn = connect()
    cursor = conn.cursor()
    
    cursor.execute('SELECT * FROM produtos_cafeteria WHERE id=%s', (id))
    prod = cursor.fetchone()
    
    nome = prod[1]
    preco_100g = prod[2]
    maquina = prod[3]
    solido = ''
    liquido = ''

    if maquina == 'S':
        solido = 'selected'
    else:
        liquido = 'selected'
    
    if request.method == 'POST' and 'nome' in request.form and 'preco_100g' in request.form and 'maquina' in request.form:

        nome = request.form['nome']
        preco_100g = request.form['preco_100g']
        maquina = request.form['maquina']

        
        cursor.execute('UPDATE produtos_cafeteria SET nome=%s, preco_100g=%s, tipo_maquina=%s WHERE id=%s', (nome, preco_100g, maquina, id))

        conn.commit()
        
        cursor.close()
        conn.close()
        return redirect('/list_prod')

    cursor.close()
    conn.close()

    return render_template('update_prod.html', id=id, nome=nome, preco_100g=preco_100g, solido=solido, liquido=liquido)

@app.route('/confirm_delete_prod/<id>')
def confirm_delete_prod(id):
    if 'loggedin' not in session or session['role'] != "admin":
        return redirect('/login')
        

    conn = connect()
    cursor = conn.cursor()

    cursor.execute('SELECT id, nome FROM produtos_cafeteria WHERE id=%s',(id))
    prod = cursor.fetchone()
    
    cursor.close()
    conn.close()

    return render_template("confirm_delete_prod.html", nome=prod[1], id=prod[0])

@app.route('/delete_prod/<id>')
def delete_prod(id):
    if 'loggedin' not in session or session['role'] != "admin":
        return redirect('/login')
    
    conn = connect()
    cursor = conn.cursor()

    cursor.execute('DELETE FROM produtos_cafeteria WHERE id=%s', (id))
    
    conn.commit()
    
    cursor.close()
    conn.close()
    
    return redirect('/list_prod')


@app.route('/add_balance/<id>', methods=['GET','POST'])
def add_balance(id):
    if 'loggedin' not in session or session['role'] != "user":
        return redirect('/login')

    conn = connect()
    cursor = conn.cursor()

    cursor.execute('SELECT name, balance FROM users_cafeteria WHERE id = %s', (id))
    user = cursor.fetchone()

    if not user:
        cursor.close()
        conn.close()
        return "Usuário não encontrado!" #Fazer uma página??

    name = user[0]
    prev_balance = float(user[1])

    if request.method == 'POST' and 'balance' in request.form:
        add_value = float(request.form['balance'])

        # ➕ Soma ao saldo existente, não substitui
        new_balance = prev_balance + add_value

        cursor.execute('UPDATE users_cafeteria SET balance = %s WHERE id = %s', (new_balance, id))
        conn.commit()

        cursor.close()
        conn.close()

        flash('Saldo atualizado com sucesso!')
        return redirect(url_for('add_balance', id=id))

    cursor.close()
    conn.close()

    return render_template('add_balance.html', id=id, name=name, balance=prev_balance)

    
if __name__ == '__main__':
    app.run(debug=True)