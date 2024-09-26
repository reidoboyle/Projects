import time
import requests
from bs4 import BeautifulSoup
from requests_html import HTMLSession
import logging
from selenium.webdriver.chrome.options import Options
from selenium.webdriver.common.by import By
from selenium.webdriver.support.wait import WebDriverWait
from selenium.webdriver.support import expected_conditions as EC
from instascrape import Profile, scrape_posts


from selenium.webdriver.common.keys import Keys

from getpass import getpass
CLASS_NAME = "class name"
TAG_NAME = "tag name"
NO_REPLIES = "-filter:replies"
USER_NAME=""
CHROME_PATH = '/usr/bin/google-chrome'
CHROMEDRIVER_PATH = '/usr/bin/chromedriver'

# Add auth token
AUTH_TOKEN = ""

polished_links = set()
logging.basicConfig(level=logging.DEBUG)

def scroll(driver):
    prev_height = driver.execute_script("return document.body.scrollHeight")

    while True:
        collect_links(driver)
        driver.execute_script("window.scrollTo(0,document.body.scrollHeight);")

        time.sleep(2) 
        new_height = driver.execute_script("return document.body.scrollHeight")
        if new_height == prev_height:
            break
        prev_height = new_height
    
    hhh = new_height-500
    while hhh > 0:
        collect_links(driver)
        driver.execute_script("window.scrollTo(0,"+ str(hhh) + ")")
        hhh = hhh -500

def collect_links(driver):
    soup = BeautifulSoup(driver.page_source, 'html.parser')
    links = soup.find_all('a')
    #print(len(ls))
    #links = driver.find_elements(By.TAG_NAME,"a")
    for link in links:
        href = link.get("href")

        if "status" in href and "analytics" not in href and "photo" not in href and "video" not in href and USER_NAME.lower() in href.lower():
            polished_links.add(href)
    #print(tweet_links)
    #print(len(tweet_links))


def scrape(user):
    USER_NAME = user
    polished_links.clear()
    if user is None:
        return []
    words = "(enter query here)"\
    #words = "dog"
    query = words +  "(from:" + USER_NAME + ")" + NO_REPLIES + "\n"
    
    chrome_options = Options()
    chrome_options.add_argument("--headless")
    #chrome_options.add_argument("--window-size=%s" % WINDOW_SIZE)
    chrome_options.binary_location = CHROME_PATH

    driver = webdriver.Chrome(options=chrome_options)
    #driver = webdriver.Chrome()
    driver.get("https://twitter.com/")


    driver.add_cookie({"name": "auth_token", "value": AUTH_TOKEN})
    driver.implicitly_wait(10)
    driver.get("https://twitter.com/search/")

    logging.debug("AUTH SUCCESSFUL")

    WebDriverWait(driver,10).until(
        EC.visibility_of_element_located((By.TAG_NAME,"input"))
    )

    search_box = driver.find_element(By.TAG_NAME,"input")
    search_box.send_keys(query)

    WebDriverWait(driver,10).until(
        EC.visibility_of_element_located((By.TAG_NAME,"article"))
    )

    scroll(driver)
    #print(polished_links)
    driver.quit()
    return list(polished_links)

def oembed(rel_link):
    url = "https://publish.twitter.com/oembed?url="
    url += "https://twitter.com" + rel_link
    resp = requests.get(url)
    return resp.json()["html"]

def instagram(user_name):
    chrome_options = Options()
    #chrome_options.add_argument("--headless")
    chrome_options.binary_location = CHROME_PATH

    driver = webdriver.Chrome(options=chrome_options)

    SESSIONID = '414762086%3ADPVDgQrvJdfDLr%3A20%3AAYdSVqJMARS1eQDoCmFPGr8utbLkQNunEzVn6Drq_A'
    headers = {"user-agent": "Mozilla/5.0 (Linux; Android 6.0; Nexus 5 Build/MRA58N) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/87.0.4280.88 Mobile Safari/537.36 Edg/87.0.664.57",
            "cookie": f"sessionid={SESSIONID};"}
    user = Profile(user_name)
    user.scrape(headers=headers)

    posts = user.get_posts(webdriver=driver, login_first=True)

    scraped, unscraped = scrape_posts(posts, silent=False, headers=headers, pause=10)

#instagram("reidoboyle")


"""links = driver.find_elements(By.TAG_NAME,"a")
tweet_links = []
print(len(links))
for link in links:
    href = link.get_attribute("href")
    print(href)
    if "status" in href and "analytics" not in href:
        tweet_links.append(href)
print(tweet_links)
print(len(tweet_links))"""




"""#wait = WebDriverWait(driver, 10);
usr_box = WebDriverWait(driver,10).until(
    EC.visibility_of_element_located((By.TAG_NAME, "input"))
)
#usr_box = driver.find_element(By.TAG_NAME,"input")
usr_box.send_keys(usr)
WebDriverWait(driver,10).until(
    EC.element_to_be_clickable((By.CLASS_NAME,"css-18t94o4"))
)
buttons = driver.find_elements(By.CLASS_NAME,"css-18t94o4")

buttons[2].click()
WebDriverWait(driver,10).until(
    EC.visibility_of_element_located((By.TAG_NAME, "input"))
)
boxes = driver.find_elements(By.TAG_NAME,"input")
pwd_box = boxes[1]
pwd_box.send_keys(pwd)

WebDriverWait(driver,10).until(
    EC.element_to_be_clickable((By.CLASS_NAME,"css-18t94o4"))
)
buttons = driver.find_elements(By.CLASS_NAME,"css-18t94o4")
submit = buttons[3]
submit.click()

query = "(bitch OR whore OR pussy OR fag OR gay OR bastard OR dyke OR retard) (from:JohnGoodlaski)"
#time.sleep(3)
WebDriverWait(driver,10).until(
    EC.visibility_of_element_located((By.TAG_NAME,"a"))
)
boxes  = driver.find_elements(By.TAG_NAME,"a")
search = boxes[2]
search.click()


search_box = driver.find_element(By.TAG_NAME,"input")
search_box.send_keys(query)
"""




#usr_box.send_keys(usr)
#import snscrape.modules.twitter as sntwitter
#import pandas as pd


"""

#con = driver.find_element(By.NAME,'text')
#print(con)

#session = HTMLSession()

#r = session.get("https://twitter.com/search?f=top&q=(bitch%20OR%20whore%20OR%20pussy%20OR%20fag%20OR%20gay%20OR%20bastard%20OR%20dyke)%20(from%3AJohnGoodlaski)&src=typed_query")

#r.html.render()

#soup = BeautifulSoup(r.content,'html.parser')
#data = soup.find_all("div")
#print(data[0])
#resp = requests.get("https://twitter.com/search?f=top&q=(bitch%20OR%20whore%20OR%20pussy%20OR%20fag%20OR%20gay%20OR%20bastard%20OR%20dyke)%20(from%3AJohnGoodlaski)&src=typed_query")
#soup = BeautifulSoup(resp.content,'html.parser')
#data = soup.find_all("div")

#print(data[0])
"""
"""
api_key="1DUAvAQ4mMAxKAXLfBriTwwmI"
api_secret="7e0Rj8iZsdXmLcNYFVoLVECjJ6LSASwS8raAx8HKWe8fFOJHvC"
access_token="967274640-9thN17TFV7g8GSjZCs3xENCtVTTh2EAlycaBJSmY"
access_secret="cbPkSeZmqf3183iShvQbjyvoXXZUDLhdFNmb1gRlvy5tn"
bearer_token="AAAAAAAAAAAAAAAAAAAAAJJTHwEAAAAALj3%2BBeMhXp%2FfQAEkHGA28PZSOME%3DAXBobSwGec4gxz9sbyDuV774GGVoPBuC41uq3qMhSFOgDwSsgB"

auth = tweepy.OAuthHandler(api_key,api_secret)
auth.set_access_token(access_token,access_secret)
 
api = tweepy.API(auth)

try:
    api.verify_credentials()
    print('Successful Authentication')
except:
    print('Failed authentication')


tweets = api.user_timeline(user_id='JohnGoodlaski')
print(tweets[0].text)
print(len(tweets))
#print(tweets[0:1000])
#json_tweets = json.loads(tweets)

"""

