(import /build/mustache)

# example from official website
(assert (= (mustache/render `
<h1>{{header}}</h1>
{{#bug}}
{{/bug}}

{{#items}}
  {{#first}}
    <li><strong>{{name}}</strong></li>
  {{/first}}
  {{#link}}
    <li><a href="{{url}}">{{name}}</a></li>
  {{/link}}
{{/items}}

{{#empty}}
  <p>The list is empty.</p>
{{/empty}}
`
`
{
  "header": "Colors",
  "items": [
      {"name": "red", "first": true, "url": "#Red"},
      {"name": "green", "link": true, "url": "#Green"},
      {"name": "blue", "link": true, "url": "#Blue"}
  ],
  "empty": false
}
`)
           "<h1>Colors</h1>\n\n    <li><strong>red</strong></li>\n    <li><a href=\"#Green\">green</a></li>\n    <li><a href=\"#Blue\">blue</a></li>\n\n"))
