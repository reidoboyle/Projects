from flask import Flask, render_template, request, redirect, url_for
from twitter import scrape,oembed

app = Flask(__name__)
RESULT_HTML = [""]

@app.route('/', methods = ["GET"])
def home():
    return render_template('home.html',pageTitle="Landing Zone")

#These functions will run when POST method is used.
@app.route('/results', methods = ["POST"] )
def submit():
    #gathering file from form
    
    user_name = request.form.get('user_name')
    #making sure its not empty
    if user_name != "":

        bad_tweets = scrape(user_name)
        RESULT_HTML[0] = '<html lang="en"><head><meta charset="UTF-8"><meta name="viewport" content="width=device-width, initial-scale=1.0"><script async src="https://platform.twitter.com/widgets.js" charset="utf-8"></script>'
        RESULT_HTML[0] += '<link rel="stylesheet" type="text/css" href="/static/css/styles.css">' + "</head>"
        RESULT_HTML[0] += "<body><h1>Tweets we think you should review</h1>"
        RESULT_HTML[0] += "<div class='main_content'>"
        for tweet in bad_tweets:
            try:
                RESULT_HTML[0] += oembed(tweet)
            except:
                continue

        RESULT_HTML[0] += "</div></body></html>"
        #return redirect(ret)
        return  redirect(url_for("results"))
       
    
    else:
        return redirect(url_for("home"))

    
      #This just reloads the page if no file is selected and the user tries to POST. 

@app.route('/results', methods = ["GET"] )
def results():
    return RESULT_HTML[0]


if __name__ == '__main__':
    app.run(ssl_context=('../../localcert/localhost.pem', '../../localcert/localhost-key.pem'),debug=True)



