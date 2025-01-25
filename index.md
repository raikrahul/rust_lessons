---
layout: default
title: Welcome to Rust Lessons
---

# Rust Lessons
Welcome to **Rust Lessons**, your ultimate resource for learning the Rust programming language.

## Latest Lessons
{% for post in site.posts %}
- [{{ post.title }}]({{ post.permalink | relative_url }})  <-- Updated to use permalink
{% endfor %}

## What You'll Learn
- Rust fundamentals
- Memory management and ownership
- Working with data types, loops, and functions
- Advanced topics like concurrency and error handling
- Real-world examples and best practices

Happy coding!