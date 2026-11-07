---
name: anki-cards
description: Turn the C study notes in notes.md into Anki flashcards for the Obsidian Flashcards v2 plugin. Use when the user asks to make, update, or sync flashcards or an Anki deck from their notes.
---

# Notes to Anki flashcards

Source: `./notes.md`
Output: `./C Flashcards.md`

The output is a note in the user's Obsidian vault. The Flashcards plugin syncs it to Anki, and Obsidian is the source of truth.

## Which notes to process

Notes that already have cards end their first line with the marker `<!-- carded -->`. This applies to topic bullets and numbered Review items alike. For an entry with sub-bullets, the marker goes on the top-level line.

- Process only entries **without** the marker. Skip marked entries entirely.
- An unmarked entry may be new, or it may be an old one whose marker was removed because the note changed. Check the existing cards for one that covers it. If one does and is still right, add nothing. If one does but is now wrong, edit it in place.
- After writing the cards, append ` <!-- carded -->` to the first line of each entry you processed, including any that needed no new card. Don't change the entry's text otherwise.

## Update, never regenerate

After the user syncs, the plugin writes a `^q-xxxx` anchor after each card and a `flashcards` property in the frontmatter. These link cards to Anki notes and their review history.

- If the output file exists, read it first. Leave every existing card, anchor, and frontmatter property exactly as it is.
- Add cards only for notes that no existing card covers. Compare by meaning, not wording.
- If a note was corrected and an existing card is now wrong, edit that card's text in place. Keep its anchor.
- Never write `^q-` anchors or the `flashcards` property yourself.
- Never delete a card unless the user asks. Deleting one deletes its review history in Anki.

## File layout

```markdown
---
cards-deck: Programming::C
tags:
  - c
  - learning-path
---

# Memory model

What creates a stack frame?::A function call. The frame is popped when the function returns.

## Read `char *(*f)(int);` aloud #card

`f` is a pointer to a function taking an `int` and returning `char *`.

# Mistakes I made

Does "indirection" create a stack frame?::No. Function calls do. Indirection means accessing through a pointer.
```

- Use `#` headings for topic sections. The plugin shows parent headings as context above each question, so name them clearly.
- In each section, put one-line cards first and heading cards after them. A heading card's answer runs until the next heading, so a one-line card placed after one would be swallowed into its answer.
- The final section is `# Mistakes I made`, built from the "Review" list in notes.md.

## Choosing a card form

- **Basic** `Question::Answer` on one line. This is the default for short answers.
- **Heading card** `## Question #card` followed by a body. Use it when the answer needs code blocks, lists, or more than about two sentences.
- **Cloze** `==hidden==` in a sentence. Use it for lists of named parts, like memory sections. Avoid `==` anywhere else in prose.
- **Reversed** `A:::B`. Use it only when recalling in both directions is genuinely useful. That's rare here.
- Never put `::` in card text outside code spans. `:::` makes a reversed card.

## Writing good cards

- One fact per card. Split notes that hold several facts.
- Ask for retrieval ("What does X do?", "Why does Y fail?"). Avoid yes/no questions, except in mistake cards that confront a specific wrong belief.
- Keep answers short. Put the key term or code first.
- Use C code in backticks and fenced `c` blocks.
- Mistake cards: phrase the question around the wrong belief, and have the answer give the correction. Don't duplicate a topic card that asks the same thing.
- Use only what the notes and the conversation established. Don't add new material.

## Finish

Report how many cards you added or changed, in which sections, and how many notes you marked as carded. Remind the user to run **Flashcards: Update Anki from current note** in Obsidian with Anki open. Don't try to reach AnkiConnect yourself.
