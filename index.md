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

Dive into real-world Rust code. Master fundamentals, control flow, and best practices by example.


## 🌟 Why Rust?
Rust is fast, reliable, and loved by developers worldwide.

## 💡 Start  Now!
Click on any lesson above.