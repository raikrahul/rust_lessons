---
layout: default
title: Welcome to Rust Lessons
---

# Welcome to Rust Lessons

## 🚀 Latest Lessons
{% for post in site.posts %}
- [{{ post.title }}]({{ post.url | relative_url }}) - *{{ post.date | date: "%B %d, %Y" }}* <!-- Changed permalink → url -->
{% endfor %}


## Learn Rust by Doing

Learn rust by staring at code, comparing code, and writing code in some other language too. 
Our goal should be to do something useful, not being lawyers about  ifs and traits. 


## 🌟 Why Rust?
Rust is fast, reliable, and loved by developers worldwide.

## 💡 Start  Now!
Click on any lesson above.